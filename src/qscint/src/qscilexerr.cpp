
#include "Qsci/qsciLexerr.h"

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>


// The ctor.
QsciLexerR::QsciLexerR(QObject* parent)
    : QsciLexer(parent)
{
    m_commentSymbol = "#";
    m_commentStart = "\"";
    m_commentEnd = "\"";
}


// The dtor.
QsciLexerR::~QsciLexerR()
{
}


// Returns the language name.
const char* QsciLexerR::language() const
{
    return "R";
}


// Returns the lexer name.
const char* QsciLexerR::lexer() const
{
    return "r";
}

// Return the string of characters that comprise a word.
const char* QsciLexerR::wordCharacters() const
{
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$@%&";
}

// Returns the foreground colour of the text for a style.
QColor QsciLexerR::defaultColor(int style) const
{
    switch (style)
    {
    case DEFAULT:
        return QColor(0x80, 0x80, 0x80);

    case KWORD:
    case BASEKWORD:
    case OTHERKWORD:
        return QColor(0x00, 0x00, 0xff);

    case COMMENT:
        return QColor(0x00, 0x7f, 0x00);

    case NUMBER:
        return QColor(0x00, 0x7f, 0x7f);

    case IDENTIFIER:
        return QColor(0x80, 0x00, 0xff);

    case STRING:
    case STRING2:
        return QColor(0x7f, 0x00, 0x7f);

    }

    return QsciLexer::defaultColor(style);
}


// Returns the end-of-line fill for a style.
bool QsciLexerR::defaultEolFill(int style) const
{
    return QsciLexer::defaultEolFill(style);
}


// Returns the font of the text for a style.
QFont QsciLexerR::defaultFont(int style) const
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

    case KWORD:
    case BASEKWORD:
    case OTHERKWORD:
        f = QsciLexer::defaultFont(style);
        f.setBold(true);
        break;

    case STRING:
    case STRING2:
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
const char* QsciLexerR::keywords(int set)
{
    if (set == 1)
    {
        return "if else repeat while function for in next break TRUE FALSE NULL NA Inf NaN";
    }
    else if (set == 2)
    {
        return "abbreviate abline abs acf acos acosh addmargins aggregate agrep alarm alias alist all anova any aov "
            "aperm append apply approx approxfun apropos ar args arima array arrows asin asinh assign assocplot atan "
            "atanh attach attr attributes autoload autoloader ave axis backsolve barplot basename beta bindtextdomain "
            "binomial biplot bitmap bmp body box boxplot bquote break browser builtins bxp by bzfile c call cancor "
            "capabilities casefold cat category cbind ccf ceiling character charmatch chartr chol choose chull citation "
           "class close cm cmdscale codes coef coefficients col colnames colors colorspaces colours comment complex confint " 
           "conflicts contour contrasts contributors convolve cophenetic coplot cor cos cosh cov covratio cpgram crossprod " 
           "cummax cummin cumprod cumsum curve cut cutree cycle data dataentry date dbeta dbinom dcauchy dchisq de debug " 
           "debugger decompose delay deltat demo dendrapply density deparse deriv det detach determinant deviance dexp df " 
           "dfbeta dfbetas dffits dgamma dgeom dget dhyper diag diff diffinv difftime digamma dim dimnames dir dirname dist " 
           "dlnorm dlogis dmultinom dnbinom dnorm dotchart double dpois dput drop dsignrank dt dump dunif duplicated dweibull " 
           "dwilcox eapply ecdf edit effects eigen emacs embed end environment eval evalq example exists exp expression factanal " 
           "factor factorial family fft fifo file filter find fitted fivenum fix floor flush for force formals format formula " 
           "forwardsolve fourfoldplot frame frequency ftable function gamma gaussian gc gcinfo gctorture get getenv geterrmessage "
           "gettext gettextf getwd gl glm globalenv gray grep grey grid gsub gzcon gzfile hat hatvalues hcl hclust head heatmap " 
           "help hist history hsv httpclient iconv iconvlist identical identify if ifelse image influence inherits integer " 
           "integrate interaction interactive intersect invisible isoreg jitter jpeg julian kappa kernapply kernel kmeans knots " 
           "kronecker ksmooth labels lag lapply layout lbeta lchoose lcm legend length letters levels lfactorial lgamma library " 
           "licence license line lines list lm load loadhistory loadings local locator loess log logb logical loglin lowess " 
           "ls lsfit machine mad mahalanobis makepredictcall manova mapply match matlines matplot matpoints matrix max mean median " 
           "medpolish menu merge message methods mget min missing mode monthplot months mosaicplot mtext mvfft names napredict "
           "naprint naresid nargs nchar ncol next nextn ngettext nlevels nlm nls noquote nrow numeric objects offset open optim "
           "optimise optimize options order ordered outer pacf page pairlist pairs palette par parse paste pbeta pbinom pbirthday "
           "pcauchy pchisq pdf pentagamma person persp pexp pf pgamma pgeom phyper pi pico pictex pie piechart pipe plclust plnorm "
           "plogis plot pmatch pmax pmin pnbinom png pnorm points poisson poly polygon polym polyroot postscript power ppoints "
           "ppois ppr prcomp predict preplot pretty princomp print prmatrix prod profile profiler proj promax prompt provide psigamma "
           "psignrank pt ptukey punif pweibull pwilcox q qbeta qbinom qbirthday qcauchy qchisq qexp qf qgamma qgeom qhyper qlnorm qlogis qnbinom " 
           "qnorm qpois qqline qqnorm qqplot qr qsignrank qt qtukey quantile quarters quasi quasibinomial quasipoisson quit qunif quote qweibull " 
           "qwilcox rainbow range rank raw rbeta rbind rbinom rcauchy rchisq readline real recover rect reformulate regexpr relevel remove reorder rep " 
           "repeat replace replicate replications require reshape resid residuals restart return rev rexp rf rgamma rgb rgeom rhyper rle rlnorm rlogis rm " 
           "rmultinom rnbinom rnorm round row rownames rowsum rpois rsignrank rstandard rstudent rt rug runif runmed rweibull rwilcox sample sapply save savehistory scale scan " 
           "screen screeplot sd search searchpaths seek segments seq sequence serialize setdiff setequal setwd shell sign signif sin single sinh sink smooth solve sort " 
           "source spectrum spline splinefun split sprintf sqrt stack stars start stderr stdin stdout stem step stepfun stl stop stopifnot str strftime strheight stripchart strptime " 
           "strsplit strtrim structure strwidth strwrap sub subset substitute substr substring sum summary sunflowerplot supsmu svd sweep switch symbols symnum system t table tabulate " 
           "tail tan tanh tapply tempdir tempfile termplot terms tetragamma text time title toeplitz tolower topenv toupper trace traceback transform trigamma trunc truncate try ts tsdiag " 
           "tsp typeof unclass undebug union unique uniroot unix unlink unlist unname unserialize unsplit unstack untrace unz update upgrade url var varimax vcov vector version vi vignette "
            "warning warnings weekdays weights which while "
           "window windows with write wsbrowser xedit xemacs xfig xinch xor xtabs xyinch yinch zapsmall";
    }
    else if (set == 3)
    {
        return "acme aids aircondit amis aml banking barchart barley beaver bigcity boot brambles breslow bs bwplot calcium cane "
            "capability cav censboot channing city claridge cloth cloud coal condense contourplot control corr darwin densityplot "
            "dogs dotplot ducks empinf envelope environmental ethanol fir frets gpar grav gravity grob hirose histogram islay knn "
            "larrows levelplot llines logit lpoints lsegments lset ltext lvqinit lvqtest manaus melanoma motor multiedit neuro "
            "nitrofen nodal ns nuclear oneway parallel paulsen poisons polar qq qqmath remission rfs saddle salinity shingle "
            "simplex singer somgrid splom stripplot survival tau tmd tsboot tuna unit urine viewport wireframe wool xyplot";
    }

}


// Returns the user name of a style.
QString QsciLexerR::description(int style) const
{
    switch (style)
    {
    case DEFAULT:
        return tr("Default");

    case COMMENT:
        return tr("Comment");

    case KWORD:
        return tr("Kword");

    case BASEKWORD:
        return tr("Basekword");

    case OTHERKWORD:
        return tr("OtherKeyWord");

    case NUMBER:
        return tr("Number");

    case STRING: //key word
        return tr("String");

    case STRING2:
        return tr("String2");

    case OPERATOR:
        return tr("Operator");

    case IDENTIFIER:
        return tr("Identifier");

    case INFIX:
        return tr("Infix");

    case INFIXEOL:
        return tr("Infix Eol");
    }

    return QString();
}


// Returns the background colour of the text for a style.
QColor QsciLexerR::defaultPaper(int style) const
{
    return QsciLexer::defaultPaper(style);
}
