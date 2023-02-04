// This module implements the QsciLexerAsm class.
//
// Copyright (c) 2021 Riverbank Computing Limited <info@riverbankcomputing.com>
// 
// This file is part of QScintilla.
// 
// This file may be used under the terms of the GNU General Public License
// version 3.0 as published by the Free Software Foundation and appearing in
// the file LICENSE included in the packaging of this file.  Please review the
// following information to ensure the GNU General Public License version 3.0
// requirements will be met: http://www.gnu.org/copyleft/gpl.html.
// 
// If you do not wish to use this file under the terms of the GPL version 3.0
// then you may purchase a commercial license.  For more information contact
// info@riverbankcomputing.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.


#include "Qsci/qscilexerasm.h"

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>


// The ctor.
QsciLexerAsm::QsciLexerAsm(QObject* parent)
    : QsciLexer(parent), fold_compact(true)
{
    m_commentSymbol = "#";
}


// The dtor.
QsciLexerAsm::~QsciLexerAsm()
{
}


// Returns the language name.
const char* QsciLexerAsm::language() const
{
    return "Asm";
}


// Returns the lexer name.
const char* QsciLexerAsm::lexer() const
{
    return "asm";
}

// Return the string of characters that comprise a word.
const char* QsciLexerAsm::wordCharacters() const
{
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$@%&";
}

// Returns the foreground colour of the text for a style.
QColor QsciLexerAsm::defaultColor(int style) const
{
    switch (style)
    {
    case DEFAULT:
        return QColor(0x80, 0x80, 0x80);

    case REGISTER:
        return QColor(0x80, 0x80, 0xff);

    case COMMENT:
        return QColor(0x00, 0x7f, 0x00);

    case NUMBER:
        return QColor(0x00, 0x7f, 0x7f);

    case IDENTIFIER:
        return QColor(0x80, 0x00, 0xff);

    case STRINGEOL:
        return QColor(0x7f, 0x00, 0x7f);

    case CPUINSTRUCTION: //key word:
        return QColor(0x00, 0x00, 0xff);
    }

    return QsciLexer::defaultColor(style);
}


// Returns the end-of-line fill for a style.
bool QsciLexerAsm::defaultEolFill(int style) const
{
    return QsciLexer::defaultEolFill(style);
}


// Returns the font of the text for a style.
QFont QsciLexerAsm::defaultFont(int style) const
{
    QFont f;

    switch (style)
    {
    case COMMENT:
#if defined(Q_OS_WIN)
        f = QFont("Courier New", 11);
#elif defined(Q_OS_MAC)
        f = QFont("Comic Sans MS", 12);
#else
        f = QFont("Bitstream Vera Serif", 9);
#endif
        break;

    case REGISTER:
        f = QsciLexer::defaultFont(style);
        f.setBold(false);
        break;

    case CPUINSTRUCTION:
        f = QsciLexer::defaultFont(style);
        f.setBold(true);
        break;

    case STRINGEOL:
    case DEFAULT:
#if defined(Q_OS_WIN)
        f = QFont("Courier New", QsciLexer::s_defaultFontSize);
#elif defined(Q_OS_MAC)
        f = QFont("Courier", 12);
#else
        f = QFont("Bitstream Vera Sans Mono", 9);
#endif
        break;

    default:
        f = QsciLexer::defaultFont(style);
    }

    return f;
}


// Returns the set of keywords.
const char* QsciLexerAsm::keywords(int set)
{
    if (set == 1)
    {
        return "aaa aad aam aas adc add and call cbw cdqe clc cld cli cmc cmp cmps cmpsb cmpsw cwd daa das dec div esc hlt idiv imul "
            "in inc int into iret ja jae jb jbe jc jcxz je jg jge jl jle jmp jna jnae jnb jnbe jnc jne jng jnge jnl jnle jno jnp jns "
            "jnz jo jp jpe jpo js jz lahf lds lea les lods lodsb lodsw loop loope loopew loopne loopnew loopnz loopnzw loopw loopz "
            "loopzw mov movabs movs movsb movsw mul neg nop not or out pop popf push pushf rcl rcr ret retf retn rol ror sahf sal sar "
            "sbb scas scasb scasw shl shr stc std sti stos stosb stosw sub test wait xchg xlat xlatb xor bound enter ins insb insw "
            "leave outs outsb outsw popa pusha pushw arpl lar lsl sgdt sidt sldt smsw str verr verw clts lgdt lidt lldt lmsw ltr "
            "bsf bsr bt btc btr bts cdq cmpsd cwde insd iretd iretdf iretf jecxz lfs lgs lodsd loopd looped loopned loopnzd loopzd "
            "lss movsd movsx movsxd movzx outsd popad popfd pushad pushd pushfd scasd seta setae setb setbe setc sete setg setge setl "
            "setle setna setnae setnb setnbe setnc setne setng setnge setnl setnle setno setnp setns setnz seto setp setpe setpo sets "
            "setz shld shrd stosd bswap cmpxchg invd invlpg wbinvd xadd lock rep repe repne repnz repz cflush cpuid emms femms cmovo "
            "cmovno cmovb cmovc cmovnae cmovae cmovnb cmovnc cmove cmovz cmovne cmovnz cmovbe cmovna cmova cmovnbe cmovs cmovns cmovp "
            "cmovpe cmovnp cmovpo cmovl cmovnge cmovge cmovnl cmovle cmovng cmovg cmovnle cmpxchg486 cmpxchg8b loadall loadall286 ibts "
            "icebp int1 int3 int01 int03 iretw popaw popfw pushaw pushfw rdmsr rdpmc rdshr rdtsc rsdc rsldt rsm rsts salc smi smint smintold "
            "svdc svldt svts syscall sysenter sysexit sysret ud0 ud1 ud2 umov xbts wrmsr wrshr";
    }
    else if (set == 2)
    {
        return "f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcom fcomp fcompp fdecstp fdisi fdiv fdivp fdivr fdivrp "
           "feni ffree fiadd ficom ficomp fidiv fidivr fild fimul fincstp finit fist fistp fisub fisubr fld "
           "fld1 fldcw fldenv fldenvw fldl2e fldl2t fldlg2 fldln2 fldpi fldz fmul fmulp fnclex fndisi fneni "
           "fninit fnop fnsave fnsavew fnstcw fnstenv fnstenvw fnstsw fpatan fprem fptan frndint frstor frstorw "
           "fsave fsavew fscale fsqrt fst fstcw fstenv fstenvw fstp fstsw fsub fsubp fsubr fsubrp ftst fwait "
           "fxam fxch fxtract fyl2x fyl2xp1 fsetpm fcos fldenvd fnsaved fnstenvd fprem1 frstord fsaved fsin "
           "fsincos fstenvd fucom fucomp fucompp fcomi fcomip ffreep fcmovb fcmove fcmovbe fcmovu fcmovnb "
            "fcmovne fcmovnbe fcmovnu";
    }
    else if (set == 3)
    {
        return "ah al ax bh bl bp bx ch cl cr0 cr2 cr3 cr4 cs cx dh di dl dr0 dr1 dr2 dr3 dr6 dr7 ds dx eax ebp ebx ecx edi edx es "
            "esi esp fs gs rax rbx rcx rdx rdi rsi rbp rsp r8 r9 r10 r11 r12 r13 r14 r15 r8d r9d r10d r11d r12d r13d r14d r15d r8w r9w "
            "r10w r11w r12w r13w r14w r15w r8b r9b r10b r11b r12b r13b r14b r15b si sp ss st tr3 tr4 tr5 tr6 tr7 st0 st1 st2 st3 st4 st5 st6 "
            "st7 mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7 xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15";
    }
    else if (set == 4)
    {
        return ".186 .286 .286c .286p .287 .386 .386c .386p .387 .486 .486p .8086 .8087 .alpha .break .code .const .continue .cref .data .data? .dosseg .else .elseif .endif .endw .err .err1 .err2 .errb .errdef .errdif .errdifi .erre .erridn .erridni .errnb .errndef .errnz .exit .fardata .fardata? .if .lall .lfcond .list .listall .listif .listmacro .listmacroall .model .no87 .nocref .nolist .nolistif .nolistmacro .radix .repeat .sall .seq .sfcond .stack .startup .tfcond .type .until .untilcxz .while .xall .xcref .xlist alias align assume catstr comm comment db dd df dosseg dq dt dup dw echo else elseif elseif1 elseif2 elseifb elseifdef elseifdif elseifdifi elseife elseifidn elseifidni elseifnb elseifndef end endif endm endp ends eq equ even exitm extern externdef extrn for forc ge goto group gt high highword if if1 if2 ifb ifdef ifdif ifdifi ife ifidn ifidni ifnb ifndef include includelib instr invoke irp irpc label le length lengthof local low lowword lroffset lt macro mask mod .msfloat name ne offset opattr option org %out page popcontext proc proto ptr public purge pushcontext record repeat rept seg segment short size sizeof sizestr struc struct substr subtitle subttl textequ this title type typedef union while width resb resw resd resq rest incbin times %define %idefine %xdefine %xidefine %undef %assign %iassign %strlen %substr %macro %imacro %endmacro %rotate %if %elif %else %endif %ifdef %ifndef %elifdef %elifndef %ifmacro %ifnmacro %elifmacro %elifnmacro %ifctk %ifnctk %elifctk %elifnctk %ifidn %ifnidn %elifidn %elifnidn %ifidni %ifnidni %elifidni %elifnidni %ifid %ifnid %elifid %elifnid %ifstr %ifnstr %elifstr %elifnstr %ifnum %ifnnum %elifnum %elifnnum %error %rep %endrep %exitrep %include %push %pop %repl endstruc istruc at iend alignb %arg %stacksize %local %line bits use16 use32 section absolute global common cpu import export";
    }
    else if (set == 5)
    {
        return "$ ? @b @f addr basic byte c carry? dword far far16 fortran fword near near16 overflow? parity? pascal qword real4 real8 real10 sbyte sdword sign? stdcall sword syscall tbyte vararg word zero? flat near32 far32 abs all assumes at casemap common compact cpu dotname emulator epilogue error export expr16 expr32 farstack forceframe huge language large listing ljmp loadds m510 medium memory nearstack nodotname noemulator nokeyword noljmp nom510 none nonunique nooldmacros nooldstructs noreadonly noscoped nosignextend nothing notpublic oldmacros oldstructs os_dos para private prologue radix readonly req scoped setif2 smallstack tiny use16 use32 uses a16 a32 o16 o32 nosplit $$ seq wrt small .text .data .bss %0 %1 %2 %3 %4 %5 %6 %7 %8 %9";
    }
    else if (set == 6)
    {
        return "addpd addps addsd addss andpd andps andnpd andnps cmpeqpd cmpltpd cmplepd cmpunordpd cmpnepd cmpnltpd cmpnlepd cmpordpd cmpeqps cmpltps cmpleps cmpunordps cmpneps cmpnltps cmpnleps cmpordps cmpeqsd cmpltsd cmplesd cmpunordsd cmpnesd cmpnltsd cmpnlesd cmpordsd cmpeqss cmpltss cmpless cmpunordss cmpness cmpnltss cmpnless cmpordss comisd comiss cvtdq2pd cvtdq2ps cvtpd2dq cvtpd2pi cvtpd2ps cvtpi2pd cvtpi2ps cvtps2dq cvtps2pd cvtps2pi cvtss2sd cvtss2si cvtsd2si cvtsd2ss cvtsi2sd cvtsi2ss cvttpd2dq cvttpd2pi cvttps2dq cvttps2pi cvttsd2si cvttss2si divpd divps divsd divss fxrstor fxsave ldmxscr lfence mfence maskmovdqu maskmovdq maxpd maxps paxsd maxss minpd minps minsd minss movapd movaps movdq2q movdqa movdqu movhlps movhpd movhps movd movq movlhps movlpd movlps movmskpd movmskps movntdq movnti movntpd movntps movntq movq2dq movsd movss movupd movups mulpd mulps mulsd mulss orpd orps packssdw packsswb packuswb paddb paddsb paddw paddsw paddd paddsiw paddq paddusb paddusw pand pandn pause paveb pavgb pavgw pavgusb pdistib pextrw pcmpeqb pcmpeqw pcmpeqd pcmpgtb pcmpgtw pcmpgtd pf2id pf2iw pfacc pfadd pfcmpeq pfcmpge pfcmpgt pfmax pfmin pfmul pmachriw pmaddwd pmagw pmaxsw pmaxub pminsw pminub pmovmskb pmulhrwc pmulhriw pmulhrwa pmulhuw pmulhw pmullw pmuludq pmvzb pmvnzb pmvlzb pmvgezb pfnacc pfpnacc por prefetch prefetchw prefetchnta prefetcht0 prefetcht1 prefetcht2 pfrcp pfrcpit1 pfrcpit2 pfrsqit1 pfrsqrt pfsub pfsubr pi2fd pinsrw psadbw pshufd pshufhw pshuflw pshufw psllw pslld psllq pslldq psraw psrad psrlw psrld psrlq psrldq psubb psubw psubd psubq psubsb psubsw psubusb psubusw psubsiw pswapd punpckhbw punpckhwd punpckhdq punpckhqdq punpcklbw punpcklwd punpckldq punpcklqdq pxor rcpps rcpss rsqrtps rsqrtss sfence shufpd shufps sqrtpd sqrtps sqrtsd sqrtss stmxcsr subpd subps subsd subss ucomisd ucomiss unpckhpd unpckhps unpcklpd unpcklps xorpd xorps";
    }
}


// Returns the user name of a style.
QString QsciLexerAsm::description(int style) const
{
    switch (style)
    {
    case DEFAULT:
        return tr("Default");

    case COMMENT:
        return tr("Comment");

    case NUMBER:
        return tr("Number");

    case STRING:
        return tr("String");

    case OPERATOR:
        return tr("Operator");

    case IDENTIFIER:
        return tr("Identifier");

    case CPUINSTRUCTION: //key word
        return tr("Keyword (Cpu instruction)");

    case MATHINSTRUCTION:
        return tr("Math instruction");

    case REGISTER:
        return tr("Register");

    case DIRECTIVE:
        return tr("Directive");

    case DIRECTIVEOPERAND:
        return tr("Directive Operand");

    case COMMENTBLOCK:
        return tr("Comment Block");

    case CHARACTER:
        return tr("Character");

    case STRINGEOL:
        return tr("String eol");

    case EXTINSTRUCTION:
        return tr("Extinstruction");

    case COMMENTDIRECTIVE:
        return tr("Comment Directive");
    }

    return QString();
}


// Returns the background colour of the text for a style.
QColor QsciLexerAsm::defaultPaper(int style) const
{
    return QsciLexer::defaultPaper(style);
}


// Refresh all properties.
void QsciLexerAsm::refreshProperties()
{
    setCompactProp();
}


// Set if folds are compact
void QsciLexerAsm::setFoldCompact(bool fold)
{
    fold_compact = fold;

    setCompactProp();
}


// Set the "fold.compact" property.
void QsciLexerAsm::setCompactProp()
{
    emit propertyChanged("fold.compact", (fold_compact ? "1" : "0"));
}
