# SH_GET_CONFIG_STR(CONFIG, VAR, DESCRIPTION, DEFAULT)
# ----------------------------------------------------
#
# Get a string as part of a configuration value, using AC_ARG_WITH,
# giving the DESCRIPTION to the second argument of AS_HELP_STRING and
# setting the resulting value to the given variable

AC_DEFUN([SH_GET_CONFIG_STR],
  [AC_ARG_WITH([$1],
              [AS_HELP_STRING([--with-$1=<string>], [$3 @<:@default=$4@:>@])],
              [AS_CASE(["$withval"],
                ["" | y | ye | yes | n | no], [AC_MSG_ERROR([Missing argument to --with-$1.  See --help])],
                [*], [$2="$withval"])])])

# SH_GET_CONFIG_NUM(CONFIG, VAR, DESCRIPTION, DEFAULT)
# ----------------------------------------------------
#
# Get a positive number as part of a configuration value, using AC_ARG_WITH,
# similar to SH_GET_CONFIG_STR

AC_DEFUN([SH_GET_CONFIG_NUM],
  [AC_ARG_WITH([$1],
              [AS_HELP_STRING([--with-$1=<num>], [$3 @<:@default=$4@:>@])],
              [AS_CASE(["$withval"],
                ["" | y | ye | yes | n | no], [AC_MSG_ERROR([Missing argument to --with-$1.  See --help])],
                [0 | *[^0-9]*], [AC_MSG_ERROR([Argument to --with-$1 must be a positive number])],
                [*], [$2="$withval"])])])
