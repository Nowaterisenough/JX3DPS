mod queue;
use queue::Queue;
use std::ffi::c_void;
use std::slice;

#[repr(C)]
#[derive(Clone, Copy)]
struct Line {
    begin: u32,
    count: u32,
    sequential: u8,
}
#[repr(C)]
#[derive(Clone, Copy)]
struct Condition {
    value: f64,
    yes: u32,
    no: u32,
    comparator: u8,
}
#[repr(C)]
pub struct Program {
    lines: *const Line,
    conditions: *const Condition,
    line_count: usize,
    condition_count: usize,
    scan_busy: bool,
    time_boundaries: bool,
}
#[repr(C)]
pub struct Hooks {
    reset: extern "C" fn(*mut c_void, u64),
    advance: extern "C" fn(*mut c_void, i32),
    timer: extern "C" fn(*mut c_void, u32),
    busy: extern "C" fn(*mut c_void) -> bool,
    available: extern "C" fn(*mut c_void, u32) -> bool,
    read: extern "C" fn(*mut c_void, u32) -> f64,
    cast: extern "C" fn(*mut c_void, u32) -> bool,
    boundary: extern "C" fn(*mut c_void) -> i32,
}
pub struct Engine {
    queue: Queue,
    lines: Vec<Line>,
    conditions: Vec<Condition>,
    consumed: Vec<bool>,
    scan_busy: bool,
    time_boundaries: bool,
}
// C++ hands us arrays only during construction. Rust owns its copies afterwards.
unsafe fn copy_array<T: Copy>(ptr: *const T, len: usize) -> Vec<T> {
    if len == 0 {
        Vec::new()
    } else {
        unsafe { slice::from_raw_parts(ptr, len) }.to_vec()
    }
}
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_new(
    priorities: *const u16,
    n: usize,
    program: *const Program,
) -> *mut Engine {
    let p = unsafe { &*program };
    Box::into_raw(Box::new(Engine {
        queue: Queue::new(&unsafe { copy_array(priorities, n) }),
        lines: unsafe { copy_array(p.lines, p.line_count) },
        conditions: unsafe { copy_array(p.conditions, p.condition_count) },
        consumed: vec![false; p.line_count],
        scan_busy: p.scan_busy,
        time_boundaries: p.time_boundaries,
    }))
}
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_delete(ptr: *mut Engine) {
    drop(unsafe { Box::from_raw(ptr) });
}
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_arm(ptr: *mut Engine, id: u32, at: i32) {
    unsafe { &mut (*ptr).queue }.arm(id as usize, at);
}
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_cancel(ptr: *mut Engine, id: u32) {
    unsafe { &mut (*ptr).queue }.cancel(id as usize);
}

fn scan(
    lines: &[Line],
    conditions: &[Condition],
    consumed: &mut [bool],
    ctx: *mut c_void,
    h: &Hooks,
) -> bool {
    const ACCEPT: u32 = u32::MAX;
    const REJECT: u32 = u32::MAX - 1;
    for (i, line) in lines.iter().enumerate() {
        if consumed[i] {
            continue;
        }
        let sequential = line.sequential != 0;
        if !(h.available)(ctx, i as u32) {
            if sequential {
                return false;
            }
            continue;
        }
        let mut pc = if line.count == 0 { ACCEPT } else { 0 };
        while pc < REJECT {
            let idx = line.begin + pc;
            let c = conditions[idx as usize];
            let v = (h.read)(ctx, idx);
            let matches = match c.comparator {
                0 => v < c.value,
                1 => v <= c.value,
                2 => v == c.value,
                3 => v != c.value,
                4 => v > c.value,
                5 => v >= c.value,
                _ => panic!("invalid comparison"),
            };
            pc = if matches { c.yes } else { c.no };
        }
        if pc == ACCEPT && (h.cast)(ctx, i as u32) {
            if sequential {
                consumed[i] = true;
            }
            return true;
        }
        if sequential {
            return false;
        }
    }
    false
}
#[unsafe(no_mangle)]
pub unsafe extern "C" fn rust_run(
    ptr: *mut Engine,
    ctx: *mut c_void,
    hooks: *const Hooks,
    seed: u64,
    end: i32,
) {
    // Callbacks can rearm this worker's queue. Never hold a reference to it (or
    // to the whole Engine) across a callback; the macro fields are disjoint.
    let h = unsafe { &*hooks };
    unsafe {
        (*ptr).queue.reset();
        (*ptr).consumed.fill(false);
    }
    (h.reset)(ctx, seed);
    let mut actions = 0;
    loop {
        let mut dispatched = 0;
        while let Some(id) = unsafe { (*ptr).queue.pop_due() } {
            dispatched += 1;
            assert!(dispatched <= 100000);
            (h.timer)(ctx, id as u32);
        }
        let may_scan = unsafe { (*ptr).scan_busy } || !(h.busy)(ctx);
        if may_scan
            && unsafe {
                scan(
                    &(*ptr).lines,
                    &(*ptr).conditions,
                    &mut (*ptr).consumed,
                    ctx,
                    h,
                )
            }
        {
            actions += 1;
            assert!(actions <= 10000);
            continue;
        }
        let mut next = unsafe { (*ptr).queue.next() }.unwrap_or(i32::MAX);
        if unsafe { (*ptr).time_boundaries } {
            next = next.min((h.boundary)(ctx));
        }
        if next >= end {
            (h.advance)(ctx, end);
            break;
        }
        unsafe {
            (*ptr).queue.now = next;
        }
        (h.advance)(ctx, next);
        actions = 0;
    }
}
