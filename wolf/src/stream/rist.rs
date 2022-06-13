#![allow(improper_ctypes)]

use crate::size_t;
use anyhow::{bail, Result};
use std::os::raw::{c_char, c_int, c_ushort, c_void};

// types
pub type w_rist_peer = *mut c_void;
pub type w_rist_data_block = *mut c_void;
pub type w_rist_oob_block = *const c_void;
pub type w_rist_stats = *const c_void;
pub type w_rist_ctx = *mut c_void;

// callbacks
pub type w_rist_log_callback =
    extern "C" fn(p_arg: *mut c_void, p_log_level: rist_log_level, p_msg: *const c_char) -> c_int;

pub type w_rist_auth_connect_callback = extern "C" fn(
    p_arg: *mut c_void,
    p_conn_ip: *const c_char,
    p_conn_port: c_ushort,
    p_local_ip: *const c_char,
    p_local_port: c_ushort,
    p_peer: w_rist_peer,
) -> c_int;

pub type w_rist_auth_disconnect_callback =
    extern "C" fn(p_arg: *mut c_void, p_peer: w_rist_peer) -> c_int;

pub type w_rist_connection_status_callback = extern "C" fn(
    p_arg: *mut c_void,
    p_peer: w_rist_peer,
    p_peer_connection_status: rist_connection_status,
) -> c_int;

pub type w_rist_out_of_band_callback =
    extern "C" fn(p_arg: *mut c_void, p_oob_block: w_rist_oob_block) -> c_int;

pub type w_rist_stats_callback =
    extern "C" fn(p_arg: *mut c_void, p_stats_container: w_rist_stats) -> c_int;

pub type w_receiver_data_callback =
    extern "C" fn(p_arg: *const c_void, p_data_block: w_rist_data_block) -> c_int;

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_connection_status {
    RIST_CONNECTION_ESTABLISHED = 0,
    RIST_CONNECTION_TIMED_OUT,
    RIST_CLIENT_CONNECTED,
    RIST_CLIENT_TIMED_OUT,
}

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_ctx_mode {
    RIST_SENDER_MODE = 0,
    RIST_RECEIVER_MODE,
}

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_profile {
    RIST_PROFILE_SIMPLE = 0,
    RIST_PROFILE_MAIN = 1,
    RIST_PROFILE_ADVANCED = 2,
}

#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub enum rist_log_level {
    RIST_LOG_DISABLE = -1,
    RIST_LOG_ERROR = 3,
    RIST_LOG_WARN = 4,
    RIST_LOG_NOTICE = 5,
    RIST_LOG_INFO = 6,
    RIST_LOG_DEBUG = 7,
    RIST_LOG_SIMULATE = 100,
}

// extern functions
extern "C" {
    fn w_rist_init(
        p_rist: *mut w_rist_ctx,
        p_mode: rist_ctx_mode,
        p_profile: rist_profile,
        p_loss_percentage: c_ushort,
        p_log_level: rist_log_level,
        p_log_callback: w_rist_log_callback,
    ) -> c_int;
    fn w_rist_connect(p_rist: w_rist_ctx, p_url: *const c_char) -> c_int;

    fn w_rist_set_auth_handler(
        p_rist: w_rist_ctx,
        p_arg: *mut c_void,
        p_on_auth_connect_cb: w_rist_auth_connect_callback,
        p_on_auth_disconnect_cb: w_rist_auth_disconnect_callback,
    ) -> c_int;

    fn w_rist_set_conn_status_callback(
        p_rist: w_rist_ctx,
        p_arg: *mut c_void,
        p_on_connect_status_cb: w_rist_connection_status_callback,
    ) -> c_int;

    fn w_rist_set_out_of_band_callback(
        p_rist: w_rist_ctx,
        p_arg: *mut c_void,
        p_on_out_of_band_cb: w_rist_out_of_band_callback,
    ) -> c_int;

    fn w_rist_set_stats_callback(
        p_rist: w_rist_ctx,
        p_interval: c_int,
        p_arg: *mut c_void,
        p_on_stats_cb: w_rist_stats_callback,
    ) -> c_int;

    fn w_rist_set_receiver_data_callback(
        p_rist: w_rist_ctx,
        p_on_receive_cb: w_receiver_data_callback,
        p_arg: *mut c_void,
    ) -> c_int;

    fn w_rist_init_data_block(p_block: *mut w_rist_data_block) -> c_int;
    fn w_rist_get_data_block(p_block: w_rist_data_block) -> *const c_void;
    fn w_rist_get_data_block_len(p_block: w_rist_data_block) -> size_t;
    fn w_rist_set_data_block(p_block: w_rist_data_block, p_data: *const c_void, p_data_len: size_t);
    fn w_rist_free_data_block(p_block: *mut w_rist_data_block);

    fn w_rist_write(p_rist: w_rist_ctx, p_block: w_rist_data_block) -> c_int;
    fn w_rist_read(p_rist: w_rist_ctx, p_block: *mut w_rist_data_block, p_timeout: c_int) -> c_int;

    fn w_rist_fini(p_rist: *mut w_rist_ctx);
}

#[derive(Clone)]
pub struct rist_data_block {
    pub block: w_rist_data_block,
}

impl Default for rist_data_block {
    fn default() -> Self {
        Self {
            block: std::ptr::null_mut(),
        }
    }
}

impl Drop for rist_data_block {
    fn drop(&mut self) {
        if !self.block.is_null() {
            unsafe { w_rist_free_data_block(&mut self.block) };
        }
    }
}

impl rist_data_block {
    pub fn new() -> Result<Self> {
        let mut rist_block = Self {
            block: std::ptr::null_mut(),
        };
        let ret = unsafe { w_rist_init_data_block(&mut rist_block.block) };
        if ret == 0 {
            return Ok(rist_block);
        }
        bail!("could not allocate memory for rist_data_block")
    }

    pub fn from(p_data_block: w_rist_data_block) -> Result<Self> {
        let mut rist_block = Self {
            block: p_data_block,
        };
        let ret = unsafe { w_rist_init_data_block(&mut rist_block.block) };
        if ret == 0 {
            return Ok(rist_block);
        }
        bail!("could not allocate memory for rist_data_block")
    }

    pub fn read(&mut self) -> &[u8] {
        unsafe {
            let ptr = w_rist_get_data_block(self.block);
            let len = w_rist_get_data_block_len(self.block);
            std::slice::from_raw_parts(ptr as *mut u8, len as usize)
        }
    }

    pub fn set(&mut self, p_data: &[u8]) {
        unsafe {
            w_rist_set_data_block(
                self.block,
                p_data.as_ptr() as *const c_void,
                p_data.len() as size_t,
            )
        };
    }
}

#[derive(Clone)]
pub struct rist {
    pub ctx: w_rist_ctx,
    pub url: String,
    pub mode: rist_ctx_mode,
    pub profile: rist_profile,
    pub loss_percentage: u16,
    pub log_level: rist_log_level,
}

impl Drop for rist {
    fn drop(&mut self) {
        //drop rist context
        unsafe { w_rist_fini(&mut self.ctx) };
    }
}

impl rist {
    pub fn new(
        p_mode: rist_ctx_mode,
        p_profile: rist_profile,
        p_loss_percentage: u16,
        p_log_level: rist_log_level,
        p_log_callback: w_rist_log_callback,
    ) -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            url: "".to_owned(),
            mode: p_mode,
            profile: p_profile,
            loss_percentage: p_loss_percentage as c_ushort,
            log_level: p_log_level,
        };

        // create sender/receiver context
        let ret = unsafe {
            w_rist_init(
                &mut obj.ctx,
                obj.mode,
                obj.profile,
                obj.loss_percentage,
                obj.log_level,
                p_log_callback,
            )
        };
        match ret {
            0 => Ok(obj),
            _ => {
                bail!("could not create rist {:?}", obj.mode)
            }
        }
    }

    pub fn connect(&mut self, p_url: &str) -> Result<()> {
        self.url = p_url.to_string();
        let ret = unsafe { w_rist_connect(self.ctx, self.url.as_ptr() as *const c_char) };
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not connect to {}", self.url)
            }
        }
    }

    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_auth_handler(
        &self,
        p_arg: *mut c_void,
        p_on_auth_connect_cb: w_rist_auth_connect_callback,
        p_on_auth_disconnect_cb: w_rist_auth_disconnect_callback,
    ) -> Result<()> {
        let ret = w_rist_set_auth_handler(
            self.ctx,
            p_arg,
            p_on_auth_connect_cb,
            p_on_auth_disconnect_cb,
        );
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set auth handler for {}", self.url)
            }
        }
    }

    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_conn_status_callback(
        &self,
        p_arg: *mut c_void,
        p_on_callback: w_rist_connection_status_callback,
    ) -> Result<()> {
        let ret = w_rist_set_conn_status_callback(self.ctx, p_arg, p_on_callback);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set connection status callback for {}", self.url)
            }
        }
    }

    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_out_of_band_callback(
        &self,
        p_arg: *mut c_void,
        p_on_callback: w_rist_out_of_band_callback,
    ) -> Result<()> {
        let ret = w_rist_set_out_of_band_callback(self.ctx, p_arg, p_on_callback);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set out of band callback for {}", self.url)
            }
        }
    }

    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_stats_callback(
        &self,
        p_interval: i32,
        p_arg: *mut c_void,
        p_on_callback: w_rist_stats_callback,
    ) -> Result<()> {
        let ret = w_rist_set_stats_callback(self.ctx, p_interval as c_int, p_arg, p_on_callback);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set stats callback for {}", self.url)
            }
        }
    }

    /// # Safety
    ///
    /// This function use void* for arg
    pub unsafe fn set_receiver_data_callback(
        &self,
        p_arg: *mut c_void,
        p_on_callback: w_receiver_data_callback,
    ) -> Result<()> {
        let ret = w_rist_set_receiver_data_callback(self.ctx, p_on_callback, p_arg);
        match ret {
            0 => Ok(()),
            _ => {
                bail!("could not set receiver callback for {}", self.url)
            }
        }
    }

    pub fn send(&self, p_data_block: rist_data_block) -> i32 {
        unsafe { w_rist_write(self.ctx, p_data_block.block) }
    }

    pub fn read(&self, p_data_block: &mut rist_data_block, p_timeout: i32) -> i32 {
        unsafe { w_rist_read(self.ctx, &mut p_data_block.block, p_timeout as c_int) }
    }
}