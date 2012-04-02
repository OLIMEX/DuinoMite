/*










 */
#ifdef INCLUDE_FUNCTION_DEFINES
// format:
//      void cmd_???(void)
//      void fun_???(void)
//      void op_???(void)
void    cmd_fse(void);
void    cmd_testcalloc(void);
#endif

#ifdef INCLUDE_COMMAND_TABLE
// the format is:
//    TEXT      	TYPE                P  FUNCTION TO CALL
// where type is always T_CMD
// and P is the precedence (which is only used for operators and not commands)

	{ "FSE",		T_CMD,				0, cmd_fse,	},
        { "TESTCALLOC",		T_CMD,				0, cmd_testcalloc,	},
#endif


