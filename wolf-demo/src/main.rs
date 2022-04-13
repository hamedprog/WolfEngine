#![feature(async_closure)]
#![allow(unused_imports)]
#![allow(unreachable_code)]

use wolf::{
    render::{w_graphics_device::WGraphicsDevice, w_scene::WScene},
    system::{chrono::w_gametime::WGameTime, os::w_runtime::WRunTime, script::w_rhai::WRhai},
    w_log,
};

#[cfg(target_arch = "wasm32")]
use {
    wasm_bindgen::prelude::*, wasm_mt::prelude::*, wolf::system::script::w_javascript::WJavaScript,
};

// Normal function
fn add(x: i64, y: i64) -> i64 {
    x + y
}

fn load(_p_gdevice: &mut WGraphicsDevice) -> anyhow::Result<()> {
    w_log!("scene just loaded");
    Ok(())
}

fn render(_p_gdevice: &mut WGraphicsDevice, _p_game_time: &mut WGameTime) -> anyhow::Result<()> {
    w_log!("scene is rendering");
    Ok(())
}

async fn app() {
    let mut script = WRhai::new();

    // register add function for our embedded script
    script.register_function("add", add);

    let res = script.run_return_any::<i64>(r#"add(10, 7)"#);
    match res {
        Ok(v) => {
            w_log!("add returns: {}", v);
        }
        Err(e) => {
            w_log!("add returns error: {:?}", e);
        }
    };

    #[cfg(not(target_arch = "wasm32"))]
    {
        let f = async move || {
            println!("t1 started");
            WRunTime::sleep(std::time::Duration::from_secs(1));
            w_log!("t1 just stopped after 2 seconds");
        };
        // execute thread
        WRunTime::green_thread(f()).await;
        WRunTime::async_sleep(std::time::Duration::from_secs(2)).await;
    }

    #[cfg(target_arch = "wasm32")]
    {
        let f1 = async move {
            let js = WJavaScript::new(None);
            let _js_res = js
                .execute(
                    "
             console.log(\"hello from javascript promise\");
             const sub = (a, b) => new Promise(resolve => {
                 setTimeout(() => resolve(a - b), 1000);
             });
             return await sub(1, 2);
         "
                    .to_owned(),
                    true,
                )
                .await;
        };
        WRuntime::spawn_local(f1);

        #[cfg(target_arch = "wasm32")]
        let f2 = FnOnce!(async move || {
            w_log!("t1 worker started");
            WRuntime::async_sleep(std::time::Duration::from_secs(2)).await;
            w_log!("t1 worker just stopped after 5 seconds");
            Ok(JsValue::null())
        });
        // execute thread
        WRuntime::thread(f2);
    }

    use wolf::system::os::w_runtime::WRunTime;
    use wolf::system::os::w_sigslot::WSigSlot;
    use wolf::w_log;

    // create SigSlot
    let mut sig_slot = WSigSlot::new();

    // create slots
    let i = 1;
    let con_1 = sig_slot.connect(move || {
        w_log!("hello from slot{}", i);
    });
    let con_2 = sig_slot.connect(|| {
        w_log!("hello from slot2");
    });

    // check for connections
    if con_1.is_connected() && con_2.is_connected() {
        w_log!("slot 1 & 2 was connected");
        // disconnect slot 2
        con_2.disconnect();
        w_log!("slot 2 just disconnected");
    }

    // wait for threads
    WRunTime::async_sleep(std::time::Duration::from_secs(1)).await;
    // emit all
    sig_slot.emit();

    // run the test scene
    use wolf::render::w_scenes_manager::WScenesManager;

    let _scene = WScene::new("hello world", &mut load, &mut render);
    //let _sm = WScenesManager::run(&mut scene).await;
}

#[cfg(target_arch = "wasm32")]
#[wasm_bindgen]
pub fn main() {
    w_log!("starting wolf-demo in wasm mode");
    std::panic::set_hook(Box::new(console_error_panic_hook::hook));
    WRuntime::spawn_local(async {
        app().await;
    });
}

#[cfg(not(target_arch = "wasm32"))]
#[tokio::main]
pub async fn main() {
    w_log!("starting wolf-demo in native mode");
    app().await;
}
