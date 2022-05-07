/* automatically generated by rust-bindgen 0.59.2 */

pub type __darwin_size_t = ::std::os::raw::c_ulong;
pub type size_t = __darwin_size_t;
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct buffer {
    pub data: *mut u8,
    pub size: size_t,
}
#[test]
fn bindgen_test_layout_buffer() {
    assert_eq!(
        ::std::mem::size_of::<buffer>(),
        16usize,
        concat!("Size of: ", stringify!(buffer))
    );
    assert_eq!(
        ::std::mem::align_of::<buffer>(),
        8usize,
        concat!("Alignment of ", stringify!(buffer))
    );
    assert_eq!(
        unsafe { &(*(::std::ptr::null::<buffer>())).data as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(buffer),
            "::",
            stringify!(data)
        )
    );
    assert_eq!(
        unsafe { &(*(::std::ptr::null::<buffer>())).size as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(buffer),
            "::",
            stringify!(size)
        )
    );
}
extern "C" {
    #[doc = " allocate buffer"]
    #[doc = " @return allocated buffer"]
    pub fn w_malloc(p_size: size_t) -> *mut buffer;
}
extern "C" {
    #[doc = " free memory of buffer"]
    #[doc = " @param p_mem is the memory buffer"]
    pub fn w_free(p_mem: *mut buffer);
}
