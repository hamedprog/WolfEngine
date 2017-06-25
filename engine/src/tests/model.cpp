#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model::model() : 
    _mesh(nullptr),
    _shader(nullptr),
    _number_of_tris(0)
{
    //define vertex binding attributes
    this->_vertex_binding_attributes.declaration = w_vertex_declaration::USER_DEFINED;
    this->_vertex_binding_attributes.binding_attributes[0] = { Vec3, Vec2 };
    this->_vertex_binding_attributes.binding_attributes[1] = { Vec3, Vec3 };
}

model::~model()
{
    release();
}

HRESULT model::load(
    _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ w_cpipeline_model* pCPModel,
    _In_ w_render_pass& pRenderPass)
{
    if (!pGDevice || !pCPModel) return S_FALSE;

    const std::string _trace = this->name + "model::load";

    //get trasform of base object and instances objects
    this->_transform = pCPModel->get_transform();
    pCPModel->get_instances(this->_instances_transforms);
    
    //get full name 
    this->_full_name = pCPModel->get_name();
    get_searchable_name(this->_full_name);
    //get instances names
    for (auto& _iter : this->_instances_transforms)
    {
        get_searchable_name(_iter.name);
    }

    uint32_t _lod_distance_index = 1;
    const uint32_t _lod_distance_offset = 500;

    //create one big vertex buffer and index buffer from root model and LODs
    std::vector<float> _batch_vertices;
    std::vector<uint32_t> _batch_indices;

    //first store vertice and indices of root model
    std::vector<w_cpipeline_model::w_mesh*> _model_meshes;
    pCPModel->get_meshes(_model_meshes);

    uint32_t _base_vertex = 0;

    if (_model_meshes.size())
    {
        this->_bounding_box_min.x = _model_meshes[0]->bounding_box.min[0];
        this->_bounding_box_min.y = _model_meshes[0]->bounding_box.min[1];
        this->_bounding_box_min.z = _model_meshes[0]->bounding_box.min[2];

        this->_bounding_box_max.x = _model_meshes[0]->bounding_box.max[0];
        this->_bounding_box_max.y = _model_meshes[0]->bounding_box.max[1];
        this->_bounding_box_max.z = _model_meshes[0]->bounding_box.max[2];

        //create first lod information
        lod _lod;
        _lod.first_index = _batch_indices.size();// First index for this LOD
        _lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
        _lod.distance = _lod_distance_index * _lod_distance_offset;
        _lod_distance_index++;

        this->_lod_levels.push_back(_lod);

        _store_to_batch(_model_meshes, _batch_vertices, _batch_indices, _base_vertex, true);

        //load texture
      /*  w_texture::load_to_shared_textures(pGDevice, 
            content_path + L"textures/areas/" + 
            wolf::system::convert::string_to_wstring(_model_meshes[0]->textures_path), &this->_texture);*/
    }

    //append load mesh data to big vertices and indices
    std::vector<w_cpipeline_model*> _lods;
    pCPModel->get_lods(_lods);
    
    for (auto& _lod_mesh_data : _lods)
    {
        _model_meshes.clear();
        _lod_mesh_data->get_meshes(_model_meshes);

        if (_model_meshes.size())
        {
            //create first lod information
            lod _lod;
            _lod.first_index = _batch_indices.size();// First index for this LOD
            _lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
            _lod.distance = _lod_distance_index * _lod_distance_offset;
            _lod_distance_index++;

            this->_lod_levels.push_back(_lod);

            _store_to_batch(_model_meshes, _batch_vertices, _batch_indices, _base_vertex, false);
        }
    }

    if (!_batch_vertices.size())
    {
        V(S_FALSE, "Model " + this->_full_name + " does not have vertices data", _trace);
        return S_FALSE;
    }

    //create mesh
    this->_mesh = new (std::nothrow) wolf::graphics::w_mesh();
    if (!_mesh)
    {
        V(S_FALSE, "Error on allocating memory for mesh for " + this->_full_name, _trace);
        return S_FALSE;
    }

    auto _v_size = static_cast<uint32_t>(_batch_vertices.size());
    auto _hr = _mesh->load(pGDevice,
        _batch_vertices.data(),
        _v_size * sizeof(float),
        _v_size,
        _batch_indices.data(),
        _batch_indices.size());
    _mesh->set_vertex_binding_attributes(this->_vertex_binding_attributes);

    _batch_vertices.clear();
    _batch_indices.clear();

    if (_hr == S_FALSE)
    {
        V(S_FALSE, "Error on loading mesh for " + this->_full_name, _trace);
        return S_FALSE;
    }


    if (_load_buffers(pGDevice) == S_FALSE) return S_FALSE;
    if (_load_shader(pGDevice) == S_FALSE) return S_FALSE;
    if (_load_pipelines(pGDevice, pRenderPass) == S_FALSE) return S_FALSE;
    if (_load_semaphores(pGDevice) == S_FALSE) return S_FALSE;

    //load command buffer for compute shader and build it
    cs.command_buffer.load(pGDevice, 1, true, &pGDevice->vk_compute_queue);
    if (_build_compute_command_buffers(pGDevice) == S_FALSE) return S_FALSE;

    auto _num_of_verts = this->_vertices_for_moc.size();
    this->_number_of_tris = _num_of_verts / 3;

    return S_OK;
}

void model::get_searchable_name(_In_z_ const std::string& pName)
{
    std::string _search_name;
    std::vector<std::string> _splits;
    wolf::system::convert::split_string(pName, "_", _splits);
    for (size_t i = 1; i < _splits.size(); i++)
    {
        _search_name += _splits[i];
    }
    _splits.clear();

    this->_search_names.push_back(_search_name);
}

void model::_store_to_batch(
    _In_ const std::vector<wolf::content_pipeline::w_cpipeline_model::w_mesh*>& pModelMeshes,
    _Inout_ std::vector<float>& pBatchVertices,
    _Inout_ std::vector<uint32_t>& pBatchIndices,
    _Inout_ uint32_t& pBaseVertex,
    _In_ const bool& pUseForMaskedOcclusionCulling)
{
    clipspace_vertex _cv;
    for (auto& _mesh_data : pModelMeshes)
    {
        for (size_t i = 0; i < _mesh_data->indices.size(); ++i)
        {
            pBatchIndices.push_back(pBaseVertex + _mesh_data->indices[i]);
        }

        for (auto& _data : _mesh_data->vertices)
        {
            auto _pos = _data.position;
            auto _uv = _data.uv;

            //position
            pBatchVertices.push_back(_pos[0]);
            pBatchVertices.push_back(_pos[1]);
            pBatchVertices.push_back(_pos[2]);

            if (pUseForMaskedOcclusionCulling)
            {
                _cv.x = _pos[0];
                _cv.y = _pos[1];
                _cv.z = 0;
                _cv.w = _pos[2];

                this->_vertices_for_moc.push_back(_cv);
            }

            //uv
            pBatchVertices.push_back(_uv[0]);
            pBatchVertices.push_back(1 - _uv[1]);

            pBaseVertex++;
        }
    }

    if (pUseForMaskedOcclusionCulling)
    {
        this->_indices_for_moc.insert(this->_indices_for_moc.end(),
            pBatchIndices.begin(), pBatchIndices.end());
    }
}

HRESULT model::_load_shader(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    const std::string _trace = this->name + "::_load_shader";

    //load vertex shader uniform
    if (this->vs.unifrom.load(pGDevice) == S_FALSE)
    {
        V(S_FALSE, "loading vertex shader uniform for " + this->_full_name, _trace);
        return S_FALSE;
    }


    std::vector<w_shader_binding_param> _shader_params;

    w_shader_binding_param _param;
    _param.index = 0;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::VERTEX_SHADER;
    _param.buffer_info = this->vs.unifrom.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 1;
    _param.type = w_shader_binding_type::SAMPLER;
    _param.stage = w_shader_stage::FRAGMENT_SHADER;
    _param.image_info = this->_texture == nullptr ? w_texture::default_texture->get_descriptor_info() :
        this->_texture->get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 0;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->cs.instance_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 1;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->indirect.indirect_draw_commands_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 2;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::COMPUTE_SHADER;

    //load compute uniform then assign it to shader
    switch (this->cs.batch_local_size)
    {
    default:
        V(S_FALSE, "batch_local_size " + std::to_string(this->cs.batch_local_size) + " not supported" + this->_full_name, _trace);
        return S_FALSE;
    case 1:
        this->_visibilities.resize(1);
        this->cs.unifrom_x1 = new w_uniform<compute_unifrom_x1>();
        if (this->cs.unifrom_x1->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x1 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x1->get_descriptor_info();
        break;
    case 2:
        this->_visibilities.resize(2);
        this->cs.unifrom_x2 = new w_uniform<compute_unifrom_x2>();
        if (this->cs.unifrom_x2->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x2 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x2->get_descriptor_info();
        break;
    case 4:
        this->_visibilities.resize(4);
        this->cs.unifrom_x4 = new w_uniform<compute_unifrom_x4>();
        if (this->cs.unifrom_x4->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x4 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x4->get_descriptor_info();
        break;
    case 8:
        this->_visibilities.resize(8);
        this->cs.unifrom_x8 = new w_uniform<compute_unifrom_x8>();
        if (this->cs.unifrom_x8->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x8 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x8->get_descriptor_info();
        break;
    case 16:
        this->_visibilities.resize(16);
        this->cs.unifrom_x16 = new w_uniform<compute_unifrom_x16>();
        if (this->cs.unifrom_x16->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x16 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x16->get_descriptor_info();
        break;
    case 32:
        this->_visibilities.resize(32);
        this->cs.unifrom_x32 = new w_uniform<compute_unifrom_x32>();
        if (this->cs.unifrom_x32->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x32 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x32->get_descriptor_info();
        break;
    case 64:
        this->_visibilities.resize(64);
        this->cs.unifrom_x64 = new w_uniform<compute_unifrom_x64>();
        if (this->cs.unifrom_x64->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x64 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x64->get_descriptor_info();
        break;
    case 128:
        this->_visibilities.resize(128);
        this->cs.unifrom_x128 = new w_uniform<compute_unifrom_x128>();
        if (this->cs.unifrom_x128->load(pGDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x128 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x128->get_descriptor_info();
        break;
    }
    _shader_params.push_back(_param);

    _param.index = 3;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->indirect.indirect_draw_count_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 4;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->cs.lod_levels_buffers.get_descriptor_info();
    _shader_params.push_back(_param);

    //check path of shader
    auto _lod_level = this->_lod_levels.size() ? this->_lod_levels.size() - 1 : 0;
    auto _shader_name = "cull_lod_" + std::to_string(_lod_level) + "_local_size_x" + std::to_string(cs.batch_local_size) + ".comp.spv";
    auto _compute_shader_path = content_path + L"shaders/compute/" + wolf::system::convert::string_to_wstring(_shader_name);
    if (wolf::system::io::get_is_file(_compute_shader_path.c_str()) == S_FALSE)
    {
        V(S_FALSE, L"compute shader not exists on path " + _compute_shader_path, _trace);
        return S_FALSE;
    }

    //load shaders
    if (w_shader::load_shader(
        pGDevice,
        _shader_name,
        content_path + L"shaders/compute/indirect_draw.vert.spv",
        L"",
        L"",
        L"",
        content_path + L"shaders/compute/indirect_draw.frag.spv",
        _compute_shader_path,
        _shader_params,
        false,
        &this->_shader) == S_FALSE)
    {
        V(S_FALSE, "Loading shader for " + this->_full_name, _trace);
        return S_FALSE;
    }

    return S_OK;
}

HRESULT model::_load_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    const std::string _trace = this->name + "::_prepare_buffers";

    w_buffer _staging_buffers[4];
    defer _(nullptr, [&](...) 
    { 
        for (size_t i = 0; i < 4; ++i)
        {
            _staging_buffers[i].release();
        }
    });

    //set compute shader batch size
    uint32_t _draw_counts = 1 + static_cast<uint32_t>(this->_instances_transforms.size());

    //find nearest pow of 2 for compute shader local batch size
    this->cs.batch_local_size = pow(2, ceil(log(_draw_counts) / log(2)));
    this->indirect.indirect_draw_commands.resize(cs.batch_local_size);
    
    this->indirect_status.draw_count = _draw_counts;

    for (size_t i = 0; i < _draw_counts; ++i)
    {
        this->indirect.indirect_draw_commands[i].instanceCount = 1;
        this->indirect.indirect_draw_commands[i].firstInstance = i;
        // firstIndex and indexCount are written by the compute shader
    }

    uint32_t _size = (uint32_t)(_draw_counts * sizeof(VkDrawIndexedIndirectCommand));
    if (_staging_buffers[0].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }
    
    if (_staging_buffers[0].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[0].set_data(this->indirect.indirect_draw_commands.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data for staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }

    if (this->indirect.indirect_draw_commands_buffer.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    if (this->indirect.indirect_draw_commands_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[0].copy_to(this->indirect.indirect_draw_commands_buffer) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    _size = (uint32_t)sizeof(this->indirect_status);
    if (this->indirect.indirect_draw_count_buffer.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of indirect_draw_count", _trace, 3);
        return S_FALSE;
    }
    
    if (this->indirect.indirect_draw_count_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of indirect_draw_count", _trace, 3);
        return S_FALSE;
    }

    std::vector<vertex_instance_data> _vertex_instances_data(_draw_counts);
    //first one is root model
    _vertex_instances_data[0].pos[0] = this->_transform.position[0];
    _vertex_instances_data[0].pos[1] = this->_transform.position[1];
    _vertex_instances_data[0].pos[2] = this->_transform.position[2];
    
    _vertex_instances_data[0].rot[0] = this->_transform.rotation[0];
    _vertex_instances_data[0].rot[1] = this->_transform.rotation[1];
    _vertex_instances_data[0].rot[2] = this->_transform.rotation[2];
        
    //others are instances
    _size = this->_instances_transforms.size();

    //resize world view projections of root and other instances
    for (size_t i = 0; i <  _size; ++i)
    {
        auto _index = i + 1;
        _vertex_instances_data[_index].pos[0] = this->_instances_transforms[i].position[0];
        _vertex_instances_data[_index].pos[1] = this->_instances_transforms[i].position[1];
        _vertex_instances_data[_index].pos[2] = this->_instances_transforms[i].position[2];

        _vertex_instances_data[_index].rot[0] = this->_instances_transforms[i].rotation[0];
        _vertex_instances_data[_index].rot[1] = this->_instances_transforms[i].rotation[1];
        _vertex_instances_data[_index].rot[2] = this->_instances_transforms[i].rotation[2];
    }
    //root and instances
    this->_world_view_projections.resize(_size + 1);

    _size = (uint32_t)(_vertex_instances_data.size() * sizeof(vertex_instance_data));
    if (_staging_buffers[1].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[1].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[1].set_data(_vertex_instances_data.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data to staging buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (vs.instance_buffer.load(pGDevice,
        _size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (vs.instance_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }
    if (_staging_buffers[1].copy_to(vs.instance_buffer) == S_FALSE)
    {
        V(S_FALSE, "copying to device buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    _size = this->_lod_levels.size() * sizeof(lod);
    if (_staging_buffers[2].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[2].bind() == S_FALSE)
    {
        V(S_FALSE, "binding staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[2].set_data(this->_lod_levels.data()))
    {
        V(S_FALSE, "setting data to staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (cs.lod_levels_buffers.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading data to staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (cs.lod_levels_buffers.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[2].copy_to(cs.lod_levels_buffers) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    std::vector<compute_instance_data> _compute_instance_data(_draw_counts);
    for (size_t i = 0; i < _draw_counts; i++)
    {
        _compute_instance_data[i].pos = glm::vec4(_vertex_instances_data[i].pos, 0.0f);
    }

    _size = _compute_instance_data.size() * sizeof(compute_instance_data);
    if (_staging_buffers[3].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[3].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[3].set_data(_compute_instance_data.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data to staging buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (cs.instance_buffer.load(pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (cs.instance_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[3].copy_to(cs.instance_buffer) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }
    
    return S_OK;
}

HRESULT model::_load_pipelines(
    _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ w_render_pass& pRenderPass)
{
    const std::string _trace = this->name + "::_load_pipelines";

    std::vector<VkDynamicState> _dynamic_states =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    auto _descriptor_set_layout_binding = this->_shader->get_descriptor_set_layout();
    auto _hr = vs.pipeline.load(
        pGDevice,
        this->_vertex_binding_attributes,
        VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        pRenderPass.get_handle(),
        _shader->get_shader_stages(),
        &_descriptor_set_layout_binding,
        { pRenderPass.get_viewport() }, //viewports
        { pRenderPass.get_viewport_scissor() }, //viewports scissor
        _dynamic_states,
        "model_pipeline_cache",
        0,
        nullptr,
        nullptr,
        true);//depth
    if (_hr)
    {
        V(S_FALSE, "creating vertex pipeline for " + this->_full_name, _trace);
        return S_FALSE;
    }

    auto _compute_descriptor_set_layout_binding = this->_shader->get_compute_descriptor_set_layout();
    auto _compute_shader_stage = this->_shader->get_compute_shader_stage();

    if (cs.pipeline.load_compute(
        pGDevice,
        _compute_shader_stage,
        _compute_descriptor_set_layout_binding,
        6,
        "model_pipeline_cache") == S_FALSE)
    {
        V(S_FALSE, "loading compute pipeline for " + this->_full_name, _trace);
        return S_FALSE;
    }
}

HRESULT model::_load_semaphores(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    const std::string _trace = this->name + "::_load_semaphores";

    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    auto _hr = vkCreateSemaphore(pGDevice->vk_device,
        &_semaphore_create_info,
        nullptr,
        &cs.semaphore);
    if (_hr)
    {
        V(S_FALSE, "creating semaphore for command buffer of " + this->_full_name, _trace);
        return S_FALSE;
    }
}

HRESULT model::_build_compute_command_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    VkCommandBufferBeginInfo _cmd_buffer_info = {};
    _cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    auto _cmd = this->cs.command_buffer.get_command_at(0);
    vkBeginCommandBuffer(_cmd, &_cmd_buffer_info);

    auto _cmd_handle = this->indirect.indirect_draw_commands_buffer.get_handle();
    auto _size = this->indirect.indirect_draw_commands_buffer.get_descriptor_info().range;


    // Add memory barrier to ensure that the indirect commands have been consumed before the compute shader updates them
    VkBufferMemoryBarrier _buffer_barrier = {};
    _buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    _buffer_barrier.buffer = _cmd_handle;
    _buffer_barrier.size = _size;
    _buffer_barrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    _buffer_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    _buffer_barrier.srcQueueFamilyIndex = pGDevice->vk_graphics_queue.index;
    _buffer_barrier.dstQueueFamilyIndex = pGDevice->vk_compute_queue.index;

    vkCmdPipelineBarrier(
        _cmd,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        1, &_buffer_barrier,
        0, nullptr);

    auto _pipline_layout_handle = this->cs.pipeline.get_layout_handle();
    auto _desciptor_set = this->_shader->get_compute_descriptor_set();
    vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, this->cs.pipeline.get_handle());
    vkCmdBindDescriptorSets(
        _cmd,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        _pipline_layout_handle,
        0,
        1, &_desciptor_set,
        0,
        0);

    vkCmdDispatch(_cmd, (uint32_t)(this->indirect.indirect_draw_commands.size() / cs.batch_local_size), 1, 1);

    // Add memory barrier to ensure that the compute shader has finished writing the indirect command buffer before it's consumed
    _buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    _buffer_barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    _buffer_barrier.buffer = _cmd_handle;
    _buffer_barrier.size = _size;
    _buffer_barrier.srcQueueFamilyIndex = pGDevice->vk_compute_queue.index;
    _buffer_barrier.dstQueueFamilyIndex = pGDevice->vk_graphics_queue.index;

    vkCmdPipelineBarrier(
        _cmd,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        0,
        0, nullptr,
        1, &_buffer_barrier,
        0, nullptr);

    vkEndCommandBuffer(_cmd);

    return S_OK;
}

static glm::mat3 rotate_over_axis(float pAngle, glm::vec3 pAxis)
{
    using namespace glm;

    const float a = pAngle;
    const float c = cos(a);
    const float s = sin(a);

    vec3 axis = normalize(pAxis);
    vec3 temp = (1 - c) * axis;

    mat3 rotate;
    rotate[0][0] = c + temp[0] * axis[0];
    rotate[0][1] = temp[0] * axis[1] + s * axis[2];
    rotate[0][2] = temp[0] * axis[2] - s * axis[1];

    rotate[1][0] = temp[1] * axis[0] - s * axis[2];
    rotate[1][1] = c + temp[1] * axis[1];
    rotate[1][2] = temp[1] * axis[2] + s * axis[0];

    rotate[2][0] = temp[2] * axis[0] + s * axis[1];
    rotate[2][1] = temp[2] * axis[1] - s * axis[0];
    rotate[2][2] = c + temp[2] * axis[2];

    return rotate;
}

float _f = 0;
void model::pre_update(
    _In_    w_first_person_camera pCamera,
    _Inout_ MaskedOcclusionCulling** sMOC)
{
    _f += 0.01f;
    using namespace glm;
    auto _view_projection = pCamera.get_projection_view();

    glm::vec3 _pos = glm::vec3(
        this->_transform.position[0],
        this->_transform.position[1],
        this->_transform.position[2]);

    this->_world_view_projections[0] = _view_projection * glm::translate(_pos);


    ////check this->_bounding_box_min
    //this->_visibilities[0] = true;

    //glm::vec4 _transformed_vector_1 =  this->_world_view_projections[0] * glm::vec4(this->_bounding_box_min, 0.0f);
    //glm::vec4 _transformed_vector_2 =  this->_world_view_projections[0] * glm::vec4(this->_bounding_box_max, 0.0f);

    //auto _f_plans = pCamera.get_frustum_plans();
    //// Check sphere against frustum planes
    //for (int i = 0; i < 6; i++)
    //{
    //    if ((glm::dot(_transformed_vector_1, _f_plans[i]) < 0.0) && (glm::dot(_transformed_vector_2, _f_plans[i]) < 0.0))
    //    {
    //        this->_visibilities[0] = false;
    //        break;
    //    }
    //}



    if (_transformed_vertices_for_moc.size() != _vertices_for_moc.size())
    {
        _transformed_vertices_for_moc.resize(_vertices_for_moc.size());
    }

    //MaskedOcclusionCulling::TransformVertices(
    //    (float*)&this->_world_view_projections[0][0],
    //    (float*)&this->_vertices_for_moc[0],
    //    (float*)&this->_transformed_vertices_for_moc[0],
    //    _transformed_vertices_for_moc.size());

    //render root model to Masked Occlusion culling
    (*sMOC)->RenderTriangles(
        (float*)&this->_vertices_for_moc[0],
        this->_indices_for_moc.data(), 
        this->_number_of_tris,
        (float*)(&this->_world_view_projections[0][0]));

    return;
    //size_t _index = 1;
    //for (auto& _ins : this->_instances_transforms)
    //{
    //    _pos = glm::vec3(
    //        _ins.position[0],
    //        _ins.position[1],
    //        _ins.position[2]);
    //    _center_pos = _pos;// +this->_bounding_box_min) + (_pos + this->_bounding_box_max)) / glm::vec3(2);

    //    this->_world_view_projections[_index] = _view_projection *
    //        glm::translate(_center_pos) *
    //        glm::rotate(
    //            0.0f,
    //            0.0f,
    //            0.0f);
    //        //glm::mat4(
    //        //    1, 0, 0, 0,
    //        //    0, 0, -1, 0,
    //        //    0, -1, 0, 0,
    //        //    0, 0, 0, 1);//swap -y and -z

    //    auto _RE = (*sMOC)->RenderTriangles(
    //        (float*)&this->_vertices_for_moc[0],
    //        this->_indices_for_moc.data(),
    //        this->_number_of_tris,
    //        (float*)&this->_world_view_projections[_index++][0]);
    //}
}

void model::post_update(
    _Inout_ MaskedOcclusionCulling* sMOC,
    _Inout_ uint32_t& pNumberOfVisibles,
    _Inout_ uint32_t& pNumberOfOccluded,
    _Inout_ uint32_t& pNumberOfViewCulled)
{
    std::fill(this->_visibilities.begin(), this->_visibilities.end(), 0.0f);

    //check root model
    auto _culling_result = sMOC->TestTriangles(
        (float*)&this->_vertices_for_moc[0],
        this->_indices_for_moc.data(),
        this->_number_of_tris,
        (float*)(&this->_world_view_projections[0][0]));
    
    switch (_culling_result)
    {
    case MaskedOcclusionCulling::VISIBLE:
        this->_visibilities[0] = true;
        pNumberOfVisibles++;
        break;
    case MaskedOcclusionCulling::OCCLUDED:
        pNumberOfOccluded++;
        break;
    case MaskedOcclusionCulling::VIEW_CULLED:
        pNumberOfViewCulled++;
        break;
    }

    return;
    /*for (size_t i = 1; i <= this->_instances_transforms.size(); ++i)
    {
        _culling_result = sMOC->TestTriangles(
            (float*)&this->_vertices_for_moc[0],
            this->_indices_for_moc.data(),
            this->_number_of_tris,
            (float*)&this->_world_view_projections[i][0]);

        switch (_culling_result)
        {
        case MaskedOcclusionCulling::VISIBLE:
            this->_visibilities[i] = 1.0f;
            pNumberOfVisibles++;
            break;
        case MaskedOcclusionCulling::OCCLUDED:
            pNumberOfOccluded++;
            break;
        case MaskedOcclusionCulling::VIEW_CULLED:
            pNumberOfViewCulled++;
            break;
        }
    }*/
}

void model::indirect_draw(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const VkCommandBuffer& pCommandBuffer)
{
    auto _descriptor_set = this->_shader->get_descriptor_set();
    this->vs.pipeline.bind(pCommandBuffer, &_descriptor_set);

    auto _instances_size = (uint32_t)this->_instances_transforms.size();
    _mesh->draw(pCommandBuffer, this->vs.instance_buffer.get_handle(), _instances_size, true);

    if (pGDevice->vk_physical_device_features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(
            pCommandBuffer,
            this->indirect.indirect_draw_commands_buffer.get_handle(),
            0,
            this->indirect_status.draw_count,
            sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (auto j = 0; j < this->indirect.indirect_draw_commands.size(); j++)
        {
            vkCmdDrawIndexedIndirect(
                pCommandBuffer,
                this->indirect.indirect_draw_commands_buffer.get_handle(),
                j * sizeof(VkDrawIndexedIndirectCommand),
                1, sizeof(VkDrawIndexedIndirectCommand));
        }
    }

}

HRESULT model::render(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const wolf::content_pipeline::w_first_person_camera* pCamera)
{
    HRESULT _hr = S_OK;

    const std::string _trace = this->name + "::draw";
    
    //Update uniforms
    auto _camera_pos = pCamera->get_translate();

    this->vs.unifrom.data.projection_view = this->_world_view_projections[0];
    if (this->vs.unifrom.update() == S_FALSE)
    {
        _hr = S_FALSE;
        V(_hr, "updating vertex shader unifrom", _trace, 3);
    }

   switch (this->cs.batch_local_size)
    {
    case 1:
        this->cs.unifrom_x1->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x1->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x1->data.is_visible));
        _hr = this->cs.unifrom_x1->update();
        break;
    case 2:
        this->cs.unifrom_x2->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x2->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x2->data.is_visible));
        _hr = this->cs.unifrom_x2->update();
        break;
    case 4:
        this->cs.unifrom_x4->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x4->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x4->data.is_visible));
        _hr = this->cs.unifrom_x4->update();
        break;
    case 8:
        this->cs.unifrom_x8->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x8->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x8->data.is_visible));
        _hr = this->cs.unifrom_x8->update();
        break;
    case 16:
        this->cs.unifrom_x16->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x16->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x16->data.is_visible));
        _hr = this->cs.unifrom_x16->update();
        break;
    case 32:
        this->cs.unifrom_x32->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x32->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x32->data.is_visible));
        _hr = this->cs.unifrom_x32->update();
        break;
    case 64:
        this->cs.unifrom_x64->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x64->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x64->data.is_visible));
        _hr = this->cs.unifrom_x64->update();
        break;
    case 128:
        this->cs.unifrom_x128->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x128->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x128->data.is_visible));
        _hr = this->cs.unifrom_x128->update();
        break;
    }

    if (_hr == S_FALSE)
    {
        V(_hr, "updating compute shader's unifrom", _trace, 3);
    }
    
    auto _c_cmd = this->cs.command_buffer.get_command_at(0);

    VkSubmitInfo _submit_info = {};
    _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _submit_info.commandBufferCount = 1;
    _submit_info.pCommandBuffers = &_c_cmd;
    _submit_info.signalSemaphoreCount = 1;
    _submit_info.pSignalSemaphores = &this->cs.semaphore;
   
    if (vkQueueSubmit(pGDevice->vk_compute_queue.queue, 1, &_submit_info, 0))
    {
        _hr = S_FALSE;
        V(_hr, "submiting queu for " + this->_full_name, _trace, 3);
    }
    
    return _hr;
}

void model::post_render(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    // Get draw count from compute
    //auto _mapped = this->indirect.indirect_draw_count_buffer.map();
    //{
    //    memcpy(&this->indirect_status, _mapped, sizeof(this->indirect_status));
    //    this->indirect.indirect_draw_commands_buffer.flush();
    //}
    //this->indirect.indirect_draw_count_buffer.unmap();


    //logger.write("visible " + std::to_string(this->indirect_status.draw_count));
    //for (size_t i = 0; i < MAX_LOD_LEVEL + 1; i++)
    //{
    //    if (this->indirect_status.lod_count[i])
    //    {
    //        logger.write("lod " + std::to_string(i));
    //    }
    //}
}

ULONG model::release()
{
    if (this->get_is_released()) return 0;

    this->vs.unifrom.release();
    switch (this->cs.batch_local_size)
    {
    case 1:  SAFE_RELEASE(this->cs.unifrom_x1); break;
    case 2:  SAFE_RELEASE(this->cs.unifrom_x2); break;
    case 4:  SAFE_RELEASE(this->cs.unifrom_x4); break;
    case 8:  SAFE_RELEASE(this->cs.unifrom_x8); break;
    case 16: SAFE_RELEASE(this->cs.unifrom_x16); break;
    case 32: SAFE_RELEASE(this->cs.unifrom_x32); break;
    case 64: SAFE_RELEASE(this->cs.unifrom_x64); break;
    case 128:SAFE_RELEASE(this->cs.unifrom_x128); break;
    }

    this->_vertices_for_moc.clear();
    this->_indices_for_moc.clear();

    return _super::release();
}

#pragma region Getters

glm::vec3 model::get_position() const
{
    return glm::vec3(
        this->_transform.position[0],
        this->_transform.position[1],
        this->_transform.position[2]);
}

#pragma endregion