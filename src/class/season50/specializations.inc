#include "schools/axzy.inc"
#include "schools/baj.inc"
#include "schools/bxj.inc"
#include "schools/dj.inc"
#include "schools/fsj.inc"
#include "schools/fysj.inc"
#include "schools/gfj.inc"
#include "schools/hjy.inc"
#include "schools/lhj.inc"
#include "schools/mw50.inc"
#include "schools/shxj.inc"
#include "schools/tlgd.inc"
#include "schools/tx50.inc"
#include "schools/txj.inc"
#include "schools/wf.inc"
#include "schools/xcj.inc"
#include "schools/yjj.inc"
#include "schools/yly.inc"
#include "schools/ztg.inc"
#include "schools/zxg.inc"

static auto BuildBooks()
{
    return std::array{ tx50(), mw50(), axzy(), baj(),  bxj(), dj(),  fsj(), fysj(), gfj(), hjy(),
                       lhj(),  shxj(), txj(),  tlgd(), wf(),  xcj(), yjj(), yly(),  ztg(), zxg() };
}
