/*
  Copyright (C) 2016 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <emacs-module.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/random.h>

int plugin_is_GPL_compatible;

static emacs_value
Fgetrandom(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	size_t len = (size_t)(env->extract_integer(env, args[0]));
	unsigned int flags = 0;

	if (nargs == 2) {
		if (env->eq(env, args[1], env->intern(env, "random"))) {
			flags = GRND_RANDOM;
		}
	}

	void *buf = malloc(len);
	if (buf == NULL) {
		return env->intern(env, "nil");
	}

	int n = syscall(SYS_getrandom, buf, len, flags);
	if (n == -1) {
		char *errstr = strerror(errno);
		emacs_value errmsg = env->make_string(env, errstr, strlen(errstr));
		env->non_local_exit_signal(env, env->intern(env, "error"), errmsg);
		return env->intern(env, "nil");
	}

	emacs_value ret = env->make_string(env, buf, n);
	free(buf);

	return ret;
}

static emacs_value
Fgetrandom_integer(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	intmax_t val;

	int n = syscall(SYS_getrandom, &val, sizeof(intmax_t), 0);
	if (n == -1) {
		char *errstr = strerror(errno);
		emacs_value errmsg = env->make_string(env, errstr, strlen(errstr));
		env->non_local_exit_signal(env, env->intern(env, "error"), errmsg);
		return env->intern(env, "nil");
	}

	return env->make_integer(env, val & UINT_MAX);
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function (env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("getrandom", Fgetrandom, 1, 2, NULL, NULL);
	DEFUN("getrandom-integer", Fgetrandom_integer, 0, 0, NULL, NULL);

#undef DEFUN

	provide(env, "getrandom");
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
