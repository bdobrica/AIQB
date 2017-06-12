/**
 * list of available [regression] algorithms
 */
enum algorithm_ct {
	alg_none,
	alg_lr_normal_form
	};

/**
 * list of available optimization algorithms
 */
enum optimization_ct {
	opt_none,
	opt_grad_descend,
	opt_nelder_mead,
	opt_pso,
	opt_nelder_mead_pso
	};

/**
 * wrapper for available algorithms
 * @param *problem, a struct fsdb_t pointer to a problem
 * @see @struct fsdb_t
 */
void _algorithm_compute (struct fsdb_t * problem);

/**
 * wrapper for computing the regression in one point
 * @param *answer, a struct answer_t pointer to an answer
 * @see @struct answer_t
 */
void _algorithm_solve (struct answer_t * answer);
