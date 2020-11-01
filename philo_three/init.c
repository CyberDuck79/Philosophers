/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 15:38:57 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 23:01:45 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

t_error			get_params(t_params *parameters, const char **argv)
{
	if ((parameters->philo_nb = str_to_nb(argv[1])) < 2)
		return (ARGS_ERROR);
	if (parameters->philo_nb > 999)
		return (ARGS_ERROR);
	if ((parameters->tt_die = str_to_nb(argv[2])) < 1)
		return (ARGS_ERROR);
	if ((parameters->tt_eat = str_to_nb(argv[3])) < 1)
		return (ARGS_ERROR);
	parameters->tt_eat *= 1000;
	if ((parameters->tt_sleep = str_to_nb(argv[4])) < 1)
		return (ARGS_ERROR);
	parameters->tt_sleep *= 1000;
	if (!argv[5])
		parameters->must_eat_nb = 0;
	else if ((parameters->must_eat_nb = str_to_nb(argv[5])) < 1)
		return (ARGS_ERROR);
	parameters->start_time = get_time();
	return (SUCCESS);
}

static t_error	init_global_sem(t_data *state, const size_t philo_nb)
{
	if ((state->forks_sem = \
	sem_open("/forks", O_CREAT | O_EXCL, S_IRWXU, philo_nb)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	if ((state->take_sem = \
	sem_open("/take", O_CREAT | O_EXCL, S_IRWXU, 1)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	if ((state->launch_barrier_sem = \
	sem_open("/launch_barrier", O_CREAT | O_EXCL, S_IRWXU, 0)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	if ((state->write_sem = \
	sem_open("/write", O_CREAT | O_EXCL, S_IRWXU, 1)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	if ((state->death_sem = \
	sem_open("/death", O_CREAT | O_EXCL, S_IRWXU, 0)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	if ((state->eat_count_sem = \
	sem_open("/eat_count", O_CREAT | O_EXCL, S_IRWXU, 0)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	return (SUCCESS);
}

static t_error	init_philo_sem(t_philo *philo, const t_index i)
{
	char *name;

	if ((name = gen_name("/launch_", i)) == NULL)
		return (SEMAPHORE_ERROR);
	if ((philo->launch_flag_sem = \
	sem_open(name, O_CREAT | O_EXCL, S_IRWXU, 0)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	philo->launch_name = name;
	if ((name = gen_name("/monit_", i)) == NULL)
		return (SEMAPHORE_ERROR);
	if ((philo->monit_sem = \
	sem_open(name, O_CREAT | O_EXCL, S_IRWXU, 1)) == SEM_FAILED)
		return (SEMAPHORE_ERROR);
	philo->monit_name = name;
	return (SUCCESS);
}

static t_error	init_semaphores(t_data *state, const size_t philo_nb)
{
	t_index i;

	unlink_semaphores(philo_nb);
	if (init_global_sem(state, philo_nb))
		return (SEMAPHORE_ERROR);
	i = 0;
	while (i < philo_nb)
	{
		state->philo[i].forks_sem = state->forks_sem;
		state->philo[i].take_sem = state->take_sem;
		state->philo[i].write_sem = state->write_sem;
		state->philo[i].death_sem = state->death_sem;
		state->philo[i].launch_barrier_sem = state->launch_barrier_sem;
		state->philo[i].eat_count_sem = state->eat_count_sem;
		if (init_philo_sem(&state->philo[i], i))
			return (SEMAPHORE_ERROR);
		i++;
	}
	return (SUCCESS);
}

t_error			init_state(t_data *state, const t_params *parameters)
{
	t_index i;

	state->parameters = parameters;
	if (!(state->philo = \
		(t_philo*)malloc(sizeof(t_philo) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	if (!(state->pid = \
		(pid_t*)malloc(sizeof(pid_t) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	i = 0;
	while (i < parameters->philo_nb)
	{
		if (!(state->philo[i].nb = nb_to_str(i + 1)))
			return (MALLOC_ERROR);
		state->philo[i].nb_len = nb_str_len(i + 1);
		state->philo[i].parameters = parameters;
		state->philo[i].launch_flag = NOT_READY;
		i++;
	}
	return (init_semaphores(state, parameters->philo_nb));
}
