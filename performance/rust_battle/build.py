"""Build the Windows GNU Release benchmarks. RUSTC and CXX may name local compilers."""
from pathlib import Path
import argparse
import os
import subprocess

here = Path(__file__).resolve().parent
root = here.parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('out', type=Path)
args = parser.parse_args()
p = args.out.resolve()
p.mkdir(parents=True, exist_ok=True)
rustc = os.environ.get('RUSTC', 'rustc')
cxx = os.environ.get('CXX', 'g++')
rust = [rustc, '--target', 'x86_64-pc-windows-gnu', '--edition', '2024',
        '-C', 'opt-level=3', '-C', 'panic=abort', '-C', 'codegen-units=1']
cpp = [cxx, '-std=c++26', '-O3', '-DNDEBUG', '-I'+str(root)]
def run(command):
    subprocess.run(command, check=True, cwd=here)
run([*rust, 'full/main.rs', '-o', p/'full-rust.exe'])
run([*rust, '--crate-type', 'staticlib', 'lib.rs', '-o', p/'libbattle.a'])
run([*cpp, '-c', 'bench.cpp', '-o', p/'bench.o'])
for exe, flags in [('battle', []), ('capture', ['-DCAPTURE_QUEUE'])]:
    run([*cpp, *flags, 'control.cpp', p/'bench.o', p/'libbattle.a',
         '-lws2_32', '-luserenv', '-lntdll', '-lbcrypt', '-lsynchronization', '-ldbghelp',
         '-o', p/(exe+'.exe')])
run([*cpp, 'queue_workload.cpp', '-o', p/'queue-cpp.exe'])
run([*rust, 'queue_workload.rs', '-o', p/'queue-rust.exe'])
run([*cpp, 'export_default.cpp', '-o', p/'export-default.exe'])
run([p/'export-default.exe', p/'tx_data.rs', p/'tx_data.hpp'])
os.environ['TX_DEFAULT_DATA'] = str(p/'tx_data.rs')
run([*rust, 'tai_xu_default.rs', '-o', p/'tx-rust.exe'])
run([*cpp, '-I'+str(p), 'tai_xu_default.cpp', '-o', p/'tx-cpp.exe'])
