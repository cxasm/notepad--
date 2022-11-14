#include "Qsci/qscilexernsis.h"
#include "Qsci/qsciscintillabase.h"

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>

QsciLexerNsis::QsciLexerNsis(QObject *parent)
	: QsciLexer(parent)
{
	
}

QsciLexerNsis::~QsciLexerNsis()
{
}

// Returns the language name.
const char* QsciLexerNsis::language() const
{
	return "NSIS";
}

// Returns the lexer name.
const char *QsciLexerNsis::lexer() const
{
	return "nsis";
}


// Returns the foreground colour of the text for a style.
QColor QsciLexerNsis::defaultColor(int style) const
{
	switch (style)
	{
	    case NSIS_COMMENT:
		case NSIS_COMMENTBOX:
			return QColor(0x00, 0x7f, 0x00);
		case NSIS_NUMBER:
			return QColor(0x00, 0x7f, 0x7f);

		case NSIS_FUNCTIONDEF:
		case NSIS_FUNCTION:
		case NSIS_MACRODEF:
		case NSIS_LABEL:
		case NSIS_SECTIONDEF:
		case NSIS_SUBSECTIONDEF:
		case NSIS_IFDEFINEDEF:
			return QColor(0x80, 0x00, 0xff);

		case NSIS_STRINGRQ:
		case NSIS_STRINGLQ:
		case NSIS_STRINGDQ:
			return QColor(0x7f, 0x00, 0x7f);
	}
	return QsciLexer::defaultColor(style);
}


// Returns the end-of-line fill for a style.
bool QsciLexerNsis::defaultEolFill(int style) const
{
	//if (style == VerbatimString)
	//	return true;

	return QsciLexer::defaultEolFill(style);
}

// Returns the font of the text for a style.
QFont QsciLexerNsis::defaultFont(int style) const
{
	QFont f;

	switch (style)
	{
	case NSIS_FUNCTIONDEF:
	case NSIS_MACRODEF:
	case NSIS_LABEL:
	case NSIS_SECTIONDEF:
	case NSIS_SUBSECTIONDEF:
	case NSIS_IFDEFINEDEF:
#if defined(Q_OS_WIN)
		f = QFont("Courier New", QsciLexer::s_defaultFontSize);
#elif defined(Q_OS_MAC)
		f = QFont("Courier", 12);
#else
		f = QFont("Bitstream Vera Sans Mono", 9);
#endif
		f.setBold(true);
		break;
	default:
		f = QsciLexer::defaultFont(style);
	}

	return f;
}


// Returns the set of keywords.
const char* QsciLexerNsis::keywords(int set)
{
	return "Functions Variables Lables UserDefined "
		"Abort AddBrandingImage AddSize AllowRootDirInstall AllowSkipFiles AutoCloseWindow "
		"BGFont BGGradient BrandingText BringToFront "
		"Call CallInstDLL Caption ChangeUI CheckBitmap ClearErrors CompletedText ComponentText CopyFiles CRCCheck CreateDirectory CreateFont CreateShortCut "
		"Delete DeleteINISec DeleteINIStr DeleteRegKey DeleteRegValue DetailPrint DetailsButtonText DirText DirVar DirVerify "
		"EnableWindow EnumRegKey EnumRegValue Exch Exec ExecShell ExecWait ExpandEnvStrings "
		"File FileBufSize FileClose FileErrorText FileOpen FileRead FileReadByte FileReadUTF16LE FileSeek FileWrite FileWriteByte FileWriteUTF16LE FindClose FindFirst FindNext FindWindow FlushINI Function FunctionEnd "
		"GetCurInstType GetCurrentAddress GetDlgItem GetDLLVersion GetDLLVersionLocal GetErrorLevel GetExeName GetExePath GetFileTime GetFileTimeLocal GetFullPathName GetFunctionAddress GetInstDirError GetLabelAddress GetTempFileName Goto "
		"HideWindow Icon IfAbort IfErrors IfFileExists IfRebootFlag IfSilent InitPluginsDir InstallButtonText InstallColors InstallDir InstallDirRegKey InstProgressFlags InstType InstTypeGetText InstTypeSetText IntCmp IntCmpU IntFmt IntOp IsWindow "
		"LangString LangStringUP LicenseBkColor LicenseData LicenseForceSelection LicenseLangString LicenseText LoadLanguageFile LockWindow LogSet LogText ManifestDPIAware ManifestSupportedOS MessageBox MiscButtonText Nop Name OutFile "
		"Page PageEx PageExEnd PluginDir Pop Push Quit ReadEnvStr ReadINIStr ReadRegDWORD ReadRegStr Reboot RegDLL Rename RequestExecutionLevel ReserveFile Return RMDir "
		"SearchPath Section SectionEnd SectionGetFlags SectionGetInstTypes SectionGetSize SectionGetText SectionGroup SectionGroupEnd SectionIn SectionSetFlags SectionSetInstTypes SectionSetSize SectionSetText SendMessage SetAutoClose SetBrandingImage SetCompress SetCompressionLevel SetCompressor SetCompressorDictSize SetCtlColors SetCurInstType SetDatablockOptimize SetDateSave SetDetailsPrint SetDetailsView SetErrorLevel SetErrors SetFileAttributes SetFont SetOutPath SetOverwrite SetPluginUnload SetRebootFlag SetRegView SetShellVarContext SetSilent SetStaticBkColor ShowInstDetails ShowUninstDetails ShowWindow SilentInstall SilentUnInstall Sleep SpaceTexts StrCmp StrCmpS StrCpy StrLen SubSection SubSectionEnd "
		"Unicode UninstallButtonText UninstallCaption UninstallIcon UninstallSubCaption UninstallText UninstPage UnRegDLL UnsafeStrCpy Var VIAddVersionKey VIFileVersion VIProductVersion WindowIcon WriteINIStr WriteRegBin WriteRegDWORD WriteRegExpandStr WriteRegStr WriteUninstaller "
		"XPStyle !AddIncludeDir !AddPluginDir !appendfile !cd !define !delfile !echo !else !endif !error !execute "
		"!finalize !getdllversion !if !ifdef !ifmacrodef !ifmacrondef !ifndef !include !insertmacro !macro !macroend !macroundef !packhdr !searchparse !searchreplace !system !tempfile !undef !verbose !warning";
}


// Returns the user name of a style.
QString QsciLexerNsis::description(int style) const
{
	switch (style)
	{
	case NSIS_DEFAULT:
		return tr("DEFAULT");
	case NSIS_COMMENT:
		return tr("COMMENT");
	case NSIS_STRINGDQ:
		return tr("STRING DOUBLE QUOTE");
	case NSIS_STRINGLQ:
		return tr("STRING LEFT QUOTE");
	case NSIS_STRINGRQ:
		return tr("STRING RIGHT QUOTE");
	case NSIS_FUNCTION:
		return tr("FUNCTION");
	case NSIS_VARIABLE:
		return tr("VARIABLE");
	case NSIS_LABEL:
		return tr("LABEL");
	case NSIS_USERDEFINED:
		return tr("USEREFINED");
	case NSIS_SECTIONDEF:
		return tr("SECTION");
	case NSIS_SUBSECTIONDEF:
		return tr("SUB SECTION");
	case NSIS_IFDEFINEDEF:
		return tr("IF DEFINE");
	case NSIS_MACRODEF:
		return tr("MACRO");
	case NSIS_STRINGVAR:
		return tr("STRING VAR");
	case NSIS_NUMBER:
		return tr("NUMBER");
	case NSIS_SECTIONGROUP:
		return tr("SECTION GROUP");
	case NSIS_PAGEEX:
		return tr("PAGE EX");
	case NSIS_FUNCTIONDEF:
		return tr("FUNCTION DEFINE");
	case NSIS_COMMENTBOX:
		return tr("COMMENT BOX");
	default:
		break;
}
	return QString();
}


// Returns the background colour of the text for a style.
QColor QsciLexerNsis::defaultPaper(int style) const
{
	//if (style == VerbatimString)
	//	return QColor(0xe0, 0xff, 0xe0);

	return QsciLexer::defaultPaper(style);
}

// Return the lexer identifier.
int QsciLexerNsis::lexerId() const
{
	return L_NSIS;
}
