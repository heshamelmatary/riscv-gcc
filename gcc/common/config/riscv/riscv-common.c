/* Common hooks for RISC-V.
   Copyright (C) 2016 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "common/common-target.h"
#include "common/common-target-def.h"
#include "opts.h"
#include "flags.h"
#include "errors.h"

/* Parse a RISC-V ISA string into an option mask.  */

static void
riscv_parse_arch_string (const char *isa, int *flags)
{
  const char *p = isa;

  if (strncasecmp (p, "RV32", 4) == 0)
    *flags |= MASK_32BIT, p += 4;
  else if (strncasecmp (p, "RV64", 4) == 0)
    *flags &= ~MASK_32BIT, p += 4;
  else if (strncasecmp (p, "RV", 2) == 0)
    p += 2;

  if (TOUPPER (*p) == 'G')
    {
      p++;

      *flags |= MASK_MUL | MASK_DIV;
      *flags |= MASK_ATOMIC;
      *flags |= MASK_HARD_FLOAT;
      *flags |= MASK_DOUBLE_FLOAT;
    }
  else if (TOUPPER (*p) == 'I')
    {
      p++;

      *flags &= ~MASK_MUL;
      if (TOUPPER (*p) == 'M')
	*flags |= MASK_MUL, p++;

      *flags &= ~MASK_ATOMIC;
      if (TOUPPER (*p) == 'A')
	*flags |= MASK_ATOMIC, p++;

      *flags &= ~(MASK_HARD_FLOAT | MASK_DOUBLE_FLOAT);
      if (TOUPPER (*p) == 'F')
	{
	  *flags |= MASK_HARD_FLOAT, p++;

	  if (TOUPPER (*p) == 'D')
	    {
	      *flags |= MASK_DOUBLE_FLOAT;
	      p++;
	    }
	}
    }
  else
    {
      error ("-march=%s: invalid ISA string", isa);
      return;
    }

  *flags &= ~MASK_RVC;
  if (TOUPPER (*p) == 'C')
    *flags |= MASK_RVC, p++;

  if (*p)
    {
      error ("-march=%s: unsupported ISA substring `%s'", isa, p);
      return;
    }
}

/* Implement TARGET_HANDLE_OPTION.  */

static bool
riscv_handle_option (struct gcc_options *opts,
		     struct gcc_options *opts_set ATTRIBUTE_UNUSED,
		     const struct cl_decoded_option *decoded,
		     location_t loc ATTRIBUTE_UNUSED)
{
  switch (decoded->opt_index)
    {
    case OPT_march_:
      riscv_parse_arch_string (decoded->arg, &opts->x_target_flags);
      return true;

    default:
      return true;
    }
}

/* Implement TARGET_OPTION_OPTIMIZATION_TABLE.  */
static const struct default_options riscv_option_optimization_table[] =
  {
    { OPT_LEVELS_1_PLUS, OPT_fsection_anchors, NULL, 1 },
    { OPT_LEVELS_1_PLUS, OPT_fomit_frame_pointer, NULL, 1 },
    { OPT_LEVELS_2_PLUS, OPT_free, NULL, 1 },
    { OPT_LEVELS_NONE, 0, NULL, 0 }
  };

#undef TARGET_OPTION_OPTIMIZATION_TABLE
#define TARGET_OPTION_OPTIMIZATION_TABLE riscv_option_optimization_table

#undef TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION riscv_handle_option

struct gcc_targetm_common targetm_common = TARGETM_COMMON_INITIALIZER;
