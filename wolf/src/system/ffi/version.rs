/* automatically generated by rust-bindgen 0.60.1 */

pub const __WORDSIZE: u32 = 64;
pub const __DARWIN_ONLY_64_BIT_INO_T: u32 = 0;
pub const __DARWIN_ONLY_UNIX_CONFORMANCE: u32 = 1;
pub const __DARWIN_ONLY_VERS_1050: u32 = 0;
pub const __DARWIN_UNIX03: u32 = 1;
pub const __DARWIN_64_BIT_INO_T: u32 = 1;
pub const __DARWIN_VERS_1050: u32 = 1;
pub const __DARWIN_NON_CANCELABLE: u32 = 0;
pub const __DARWIN_SUF_64_BIT_INO_T: &[u8; 9usize] = b"$INODE64\0";
pub const __DARWIN_SUF_1050: &[u8; 6usize] = b"$1050\0";
pub const __DARWIN_SUF_EXTSN: &[u8; 14usize] = b"$DARWIN_EXTSN\0";
pub const __DARWIN_C_ANSI: u32 = 4096;
pub const __DARWIN_C_FULL: u32 = 900000;
pub const __DARWIN_C_LEVEL: u32 = 900000;
pub const __STDC_WANT_LIB_EXT1__: u32 = 1;
pub const __DARWIN_NO_LONG_LONG: u32 = 0;
pub const _DARWIN_FEATURE_64_BIT_INODE: u32 = 1;
pub const _DARWIN_FEATURE_ONLY_UNIX_CONFORMANCE: u32 = 1;
pub const _DARWIN_FEATURE_UNIX_CONFORMANCE: u32 = 3;
pub const __has_ptrcheck: u32 = 0;
pub const __PTHREAD_SIZE__: u32 = 8176;
pub const __PTHREAD_ATTR_SIZE__: u32 = 56;
pub const __PTHREAD_MUTEXATTR_SIZE__: u32 = 8;
pub const __PTHREAD_MUTEX_SIZE__: u32 = 56;
pub const __PTHREAD_CONDATTR_SIZE__: u32 = 8;
pub const __PTHREAD_COND_SIZE__: u32 = 40;
pub const __PTHREAD_ONCE_SIZE__: u32 = 8;
pub const __PTHREAD_RWLOCK_SIZE__: u32 = 192;
pub const __PTHREAD_RWLOCKATTR_SIZE__: u32 = 16;
pub const INT8_MAX: u32 = 127;
pub const INT16_MAX: u32 = 32767;
pub const INT32_MAX: u32 = 2147483647;
pub const INT64_MAX: u64 = 9223372036854775807;
pub const INT8_MIN: i32 = -128;
pub const INT16_MIN: i32 = -32768;
pub const INT32_MIN: i32 = -2147483648;
pub const INT64_MIN: i64 = -9223372036854775808;
pub const UINT8_MAX: u32 = 255;
pub const UINT16_MAX: u32 = 65535;
pub const UINT32_MAX: u32 = 4294967295;
pub const UINT64_MAX: i32 = -1;
pub const INT_LEAST8_MIN: i32 = -128;
pub const INT_LEAST16_MIN: i32 = -32768;
pub const INT_LEAST32_MIN: i32 = -2147483648;
pub const INT_LEAST64_MIN: i64 = -9223372036854775808;
pub const INT_LEAST8_MAX: u32 = 127;
pub const INT_LEAST16_MAX: u32 = 32767;
pub const INT_LEAST32_MAX: u32 = 2147483647;
pub const INT_LEAST64_MAX: u64 = 9223372036854775807;
pub const UINT_LEAST8_MAX: u32 = 255;
pub const UINT_LEAST16_MAX: u32 = 65535;
pub const UINT_LEAST32_MAX: u32 = 4294967295;
pub const UINT_LEAST64_MAX: i32 = -1;
pub const INT_FAST8_MIN: i32 = -128;
pub const INT_FAST16_MIN: i32 = -32768;
pub const INT_FAST32_MIN: i32 = -2147483648;
pub const INT_FAST64_MIN: i64 = -9223372036854775808;
pub const INT_FAST8_MAX: u32 = 127;
pub const INT_FAST16_MAX: u32 = 32767;
pub const INT_FAST32_MAX: u32 = 2147483647;
pub const INT_FAST64_MAX: u64 = 9223372036854775807;
pub const UINT_FAST8_MAX: u32 = 255;
pub const UINT_FAST16_MAX: u32 = 65535;
pub const UINT_FAST32_MAX: u32 = 4294967295;
pub const UINT_FAST64_MAX: i32 = -1;
pub const INTPTR_MAX: u64 = 9223372036854775807;
pub const INTPTR_MIN: i64 = -9223372036854775808;
pub const UINTPTR_MAX: i32 = -1;
pub const SIZE_MAX: i32 = -1;
pub const RSIZE_MAX: i32 = -1;
pub const WINT_MIN: i32 = -2147483648;
pub const WINT_MAX: u32 = 2147483647;
pub const SIG_ATOMIC_MIN: i32 = -2147483648;
pub const SIG_ATOMIC_MAX: u32 = 2147483647;
pub const W_MAX_PATH: u32 = 260;
pub type size_t = ::std::os::raw::c_ulong;
pub type wchar_t = ::std::os::raw::c_int;
pub type max_align_t = u128;
pub type int_least8_t = i8;
pub type int_least16_t = i16;
pub type int_least32_t = i32;
pub type int_least64_t = i64;
pub type uint_least8_t = u8;
pub type uint_least16_t = u16;
pub type uint_least32_t = u32;
pub type uint_least64_t = u64;
pub type int_fast8_t = i8;
pub type int_fast16_t = i16;
pub type int_fast32_t = i32;
pub type int_fast64_t = i64;
pub type uint_fast8_t = u8;
pub type uint_fast16_t = u16;
pub type uint_fast32_t = u32;
pub type uint_fast64_t = u64;
pub type __int8_t = ::std::os::raw::c_schar;
pub type __uint8_t = ::std::os::raw::c_uchar;
pub type __int16_t = ::std::os::raw::c_short;
pub type __uint16_t = ::std::os::raw::c_ushort;
pub type __int32_t = ::std::os::raw::c_int;
pub type __uint32_t = ::std::os::raw::c_uint;
pub type __int64_t = ::std::os::raw::c_longlong;
pub type __uint64_t = ::std::os::raw::c_ulonglong;
pub type __darwin_intptr_t = ::std::os::raw::c_long;
pub type __darwin_natural_t = ::std::os::raw::c_uint;
pub type __darwin_ct_rune_t = ::std::os::raw::c_int;
#[repr(C)]
#[derive(Copy, Clone)]
pub union __mbstate_t {
    pub __mbstate8: [::std::os::raw::c_char; 128usize],
    pub _mbstateL: ::std::os::raw::c_longlong,
}
#[test]
fn bindgen_test_layout___mbstate_t() {
    assert_eq!(
        ::std::mem::size_of::<__mbstate_t>(),
        128usize,
        concat!("Size of: ", stringify!(__mbstate_t))
    );
    assert_eq!(
        ::std::mem::align_of::<__mbstate_t>(),
        8usize,
        concat!("Alignment of ", stringify!(__mbstate_t))
    );
    fn test_field___mbstate8() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__mbstate_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__mbstate8) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(__mbstate_t),
                "::",
                stringify!(__mbstate8)
            )
        );
    }
    test_field___mbstate8();
    fn test_field__mbstateL() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__mbstate_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr)._mbstateL) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(__mbstate_t),
                "::",
                stringify!(_mbstateL)
            )
        );
    }
    test_field__mbstateL();
}
pub type __darwin_mbstate_t = __mbstate_t;
pub type __darwin_ptrdiff_t = ::std::os::raw::c_long;
pub type __darwin_size_t = ::std::os::raw::c_ulong;
pub type __darwin_va_list = __builtin_va_list;
pub type __darwin_wchar_t = ::std::os::raw::c_int;
pub type __darwin_rune_t = __darwin_wchar_t;
pub type __darwin_wint_t = ::std::os::raw::c_int;
pub type __darwin_clock_t = ::std::os::raw::c_ulong;
pub type __darwin_socklen_t = __uint32_t;
pub type __darwin_ssize_t = ::std::os::raw::c_long;
pub type __darwin_time_t = ::std::os::raw::c_long;
pub type __darwin_blkcnt_t = __int64_t;
pub type __darwin_blksize_t = __int32_t;
pub type __darwin_dev_t = __int32_t;
pub type __darwin_fsblkcnt_t = ::std::os::raw::c_uint;
pub type __darwin_fsfilcnt_t = ::std::os::raw::c_uint;
pub type __darwin_gid_t = __uint32_t;
pub type __darwin_id_t = __uint32_t;
pub type __darwin_ino64_t = __uint64_t;
pub type __darwin_ino_t = __darwin_ino64_t;
pub type __darwin_mach_port_name_t = __darwin_natural_t;
pub type __darwin_mach_port_t = __darwin_mach_port_name_t;
pub type __darwin_mode_t = __uint16_t;
pub type __darwin_off_t = __int64_t;
pub type __darwin_pid_t = __int32_t;
pub type __darwin_sigset_t = __uint32_t;
pub type __darwin_suseconds_t = __int32_t;
pub type __darwin_uid_t = __uint32_t;
pub type __darwin_useconds_t = __uint32_t;
pub type __darwin_uuid_t = [::std::os::raw::c_uchar; 16usize];
pub type __darwin_uuid_string_t = [::std::os::raw::c_char; 37usize];
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct __darwin_pthread_handler_rec {
    pub __routine: ::std::option::Option<unsafe extern "C" fn(arg1: *mut ::std::os::raw::c_void)>,
    pub __arg: *mut ::std::os::raw::c_void,
    pub __next: *mut __darwin_pthread_handler_rec,
}
#[test]
fn bindgen_test_layout___darwin_pthread_handler_rec() {
    assert_eq!(
        ::std::mem::size_of::<__darwin_pthread_handler_rec>(),
        24usize,
        concat!("Size of: ", stringify!(__darwin_pthread_handler_rec))
    );
    assert_eq!(
        ::std::mem::align_of::<__darwin_pthread_handler_rec>(),
        8usize,
        concat!("Alignment of ", stringify!(__darwin_pthread_handler_rec))
    );
    fn test_field___routine() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__darwin_pthread_handler_rec>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__routine) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(__darwin_pthread_handler_rec),
                "::",
                stringify!(__routine)
            )
        );
    }
    test_field___routine();
    fn test_field___arg() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__darwin_pthread_handler_rec>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__arg) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(__darwin_pthread_handler_rec),
                "::",
                stringify!(__arg)
            )
        );
    }
    test_field___arg();
    fn test_field___next() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__darwin_pthread_handler_rec>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__next) as usize - ptr as usize
            },
            16usize,
            concat!(
                "Offset of field: ",
                stringify!(__darwin_pthread_handler_rec),
                "::",
                stringify!(__next)
            )
        );
    }
    test_field___next();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_attr_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 56usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_attr_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_attr_t>(),
        64usize,
        concat!("Size of: ", stringify!(_opaque_pthread_attr_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_attr_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_attr_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_attr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_attr_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_attr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_attr_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_cond_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 40usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_cond_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_cond_t>(),
        48usize,
        concat!("Size of: ", stringify!(_opaque_pthread_cond_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_cond_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_cond_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_cond_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_cond_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_cond_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_cond_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_condattr_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 8usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_condattr_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_condattr_t>(),
        16usize,
        concat!("Size of: ", stringify!(_opaque_pthread_condattr_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_condattr_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_condattr_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_condattr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_condattr_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_condattr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_condattr_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_mutex_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 56usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_mutex_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_mutex_t>(),
        64usize,
        concat!("Size of: ", stringify!(_opaque_pthread_mutex_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_mutex_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_mutex_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_mutex_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_mutex_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_mutex_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_mutex_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_mutexattr_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 8usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_mutexattr_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_mutexattr_t>(),
        16usize,
        concat!("Size of: ", stringify!(_opaque_pthread_mutexattr_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_mutexattr_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_mutexattr_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_mutexattr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_mutexattr_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_mutexattr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_mutexattr_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_once_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 8usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_once_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_once_t>(),
        16usize,
        concat!("Size of: ", stringify!(_opaque_pthread_once_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_once_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_once_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_once_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_once_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_once_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_once_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_rwlock_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 192usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_rwlock_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_rwlock_t>(),
        200usize,
        concat!("Size of: ", stringify!(_opaque_pthread_rwlock_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_rwlock_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_rwlock_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_rwlock_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_rwlock_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_rwlock_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_rwlock_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_rwlockattr_t {
    pub __sig: ::std::os::raw::c_long,
    pub __opaque: [::std::os::raw::c_char; 16usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_rwlockattr_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_rwlockattr_t>(),
        24usize,
        concat!("Size of: ", stringify!(_opaque_pthread_rwlockattr_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_rwlockattr_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_rwlockattr_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_rwlockattr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_rwlockattr_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_rwlockattr_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_rwlockattr_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct _opaque_pthread_t {
    pub __sig: ::std::os::raw::c_long,
    pub __cleanup_stack: *mut __darwin_pthread_handler_rec,
    pub __opaque: [::std::os::raw::c_char; 8176usize],
}
#[test]
fn bindgen_test_layout__opaque_pthread_t() {
    assert_eq!(
        ::std::mem::size_of::<_opaque_pthread_t>(),
        8192usize,
        concat!("Size of: ", stringify!(_opaque_pthread_t))
    );
    assert_eq!(
        ::std::mem::align_of::<_opaque_pthread_t>(),
        8usize,
        concat!("Alignment of ", stringify!(_opaque_pthread_t))
    );
    fn test_field___sig() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__sig) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_t),
                "::",
                stringify!(__sig)
            )
        );
    }
    test_field___sig();
    fn test_field___cleanup_stack() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__cleanup_stack) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_t),
                "::",
                stringify!(__cleanup_stack)
            )
        );
    }
    test_field___cleanup_stack();
    fn test_field___opaque() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<_opaque_pthread_t>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).__opaque) as usize - ptr as usize
            },
            16usize,
            concat!(
                "Offset of field: ",
                stringify!(_opaque_pthread_t),
                "::",
                stringify!(__opaque)
            )
        );
    }
    test_field___opaque();
}
pub type __darwin_pthread_attr_t = _opaque_pthread_attr_t;
pub type __darwin_pthread_cond_t = _opaque_pthread_cond_t;
pub type __darwin_pthread_condattr_t = _opaque_pthread_condattr_t;
pub type __darwin_pthread_key_t = ::std::os::raw::c_ulong;
pub type __darwin_pthread_mutex_t = _opaque_pthread_mutex_t;
pub type __darwin_pthread_mutexattr_t = _opaque_pthread_mutexattr_t;
pub type __darwin_pthread_once_t = _opaque_pthread_once_t;
pub type __darwin_pthread_rwlock_t = _opaque_pthread_rwlock_t;
pub type __darwin_pthread_rwlockattr_t = _opaque_pthread_rwlockattr_t;
pub type __darwin_pthread_t = *mut _opaque_pthread_t;
pub type u_int8_t = ::std::os::raw::c_uchar;
pub type u_int16_t = ::std::os::raw::c_ushort;
pub type u_int32_t = ::std::os::raw::c_uint;
pub type u_int64_t = ::std::os::raw::c_ulonglong;
pub type register_t = i64;
pub type user_addr_t = u_int64_t;
pub type user_size_t = u_int64_t;
pub type user_ssize_t = i64;
pub type user_long_t = i64;
pub type user_ulong_t = u_int64_t;
pub type user_time_t = i64;
pub type user_off_t = i64;
pub type syscall_arg_t = u_int64_t;
pub type intmax_t = ::std::os::raw::c_long;
pub type uintmax_t = ::std::os::raw::c_ulong;
extern "C" {
    #[doc = " returns wolf version"]
    #[doc = " @param p_buf buffer with maximum size of 260 characters"]
    #[doc = " @param p_len the length of buffer, maximum size is 260"]
    #[doc = " @return string format with the following style"]
    #[doc = " \"<major>.<minor>.<patch>.<debug>\""]
    pub fn w_sys_version(p_buf: *mut ::std::os::raw::c_char, p_len: size_t);
}
pub type __builtin_va_list = [__va_list_tag; 1usize];
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct __va_list_tag {
    pub gp_offset: ::std::os::raw::c_uint,
    pub fp_offset: ::std::os::raw::c_uint,
    pub overflow_arg_area: *mut ::std::os::raw::c_void,
    pub reg_save_area: *mut ::std::os::raw::c_void,
}
#[test]
fn bindgen_test_layout___va_list_tag() {
    assert_eq!(
        ::std::mem::size_of::<__va_list_tag>(),
        24usize,
        concat!("Size of: ", stringify!(__va_list_tag))
    );
    assert_eq!(
        ::std::mem::align_of::<__va_list_tag>(),
        8usize,
        concat!("Alignment of ", stringify!(__va_list_tag))
    );
    fn test_field_gp_offset() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__va_list_tag>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).gp_offset) as usize - ptr as usize
            },
            0usize,
            concat!(
                "Offset of field: ",
                stringify!(__va_list_tag),
                "::",
                stringify!(gp_offset)
            )
        );
    }
    test_field_gp_offset();
    fn test_field_fp_offset() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__va_list_tag>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).fp_offset) as usize - ptr as usize
            },
            4usize,
            concat!(
                "Offset of field: ",
                stringify!(__va_list_tag),
                "::",
                stringify!(fp_offset)
            )
        );
    }
    test_field_fp_offset();
    fn test_field_overflow_arg_area() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__va_list_tag>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).overflow_arg_area) as usize - ptr as usize
            },
            8usize,
            concat!(
                "Offset of field: ",
                stringify!(__va_list_tag),
                "::",
                stringify!(overflow_arg_area)
            )
        );
    }
    test_field_overflow_arg_area();
    fn test_field_reg_save_area() {
        assert_eq!(
            unsafe {
                let uninit = ::std::mem::MaybeUninit::<__va_list_tag>::uninit();
                let ptr = uninit.as_ptr();
                ::std::ptr::addr_of!((*ptr).reg_save_area) as usize - ptr as usize
            },
            16usize,
            concat!(
                "Offset of field: ",
                stringify!(__va_list_tag),
                "::",
                stringify!(reg_save_area)
            )
        );
    }
    test_field_reg_save_area();
}
