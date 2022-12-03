#ifndef REG_H
#define REG_H

#include <map>

enum class Reg {
    $zero,

    $at,

    $v0, $v1,
    $a0, $a1, $a2, $a3,

    $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9,

    $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7,

    $k0, $k1,

    $gp,

    $sp,

    $fp,

    $ra
};

//const Reg tempRegs[] = {Reg::$t0, Reg::$t1, Reg::$t2, Reg::$t3, Reg::$t4,
//                        Reg::$t5, Reg::$t6, Reg::$t7, Reg::$t8, Reg::$t9};
const Reg tempRegs[] = {Reg::$t0, Reg::$t1, Reg::$t2, Reg::$t9};

const std::map<Reg, std::string> reg2s = {
        {Reg::$zero, "$zero"},
        {Reg::$at,   "$at"},
        {Reg::$v0,   "$v0"},
        {Reg::$v1,   "$v1"},
        {Reg::$a0,   "$a0"},
        {Reg::$a1,   "$a1"},
        {Reg::$a2,   "$a2"},
        {Reg::$a3,   "$a3"},
        {Reg::$t0,   "$t0"},
        {Reg::$t1,   "$t1"},
        {Reg::$t2,   "$t2"},
        {Reg::$t3,   "$t3"},
        {Reg::$t4,   "$t4"},
        {Reg::$t5,   "$t5"},
        {Reg::$t6,   "$t6"},
        {Reg::$t7,   "$t7"},
        {Reg::$t8,   "$t8"},
        {Reg::$t9,   "$t9"},
        {Reg::$s0,   "$s0"},
        {Reg::$s1,   "$s1"},
        {Reg::$s2,   "$s2"},
        {Reg::$s3,   "$s3"},
        {Reg::$s4,   "$s4"},
        {Reg::$s5,   "$s5"},
        {Reg::$s6,   "$s6"},
        {Reg::$s7,   "$s7"},
        {Reg::$k0,   "$k0"},
        {Reg::$k1,   "$k1"},
        {Reg::$gp,   "$gp"},
        {Reg::$sp,   "$sp"},
        {Reg::$fp,   "$fp"},
        {Reg::$ra,   "$ra"}
};

#endif //REG_H
