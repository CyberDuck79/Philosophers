/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 13:28:38 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 23:01:22 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

static t_error	launch_philo(t_data *state, const size_t philo_nb)
{
	t_index	i;

	i = 0;
	while (i < philo_nb)
	{
		state->pid[i] = fork();
		if (state->pid[i] < 0)
			return (FORK_ERROR);
		if (!state->pid[i])
			philosopher(&state->philo[i]);
		sem_wait(state->philo[i].launch_flag_sem);
		i++;
	}
	i = 0;
	while (i < philo_nb)
	{
		sem_post(state->launch_barrier_sem);
		i++;
	}
	return (SUCCESS);
}

static t_error	start_simulation(t_data *state, const t_params *parameters)
{
	t_thread	tid;

	if (parameters->must_eat_nb > 0)
	{
		if (pthread_create(&tid, NULL, &eat_monitor, (void*)state))
			return (THREAD_ERROR);
		pthread_detach(tid);
	}
	if (launch_philo(state, parameters->philo_nb))
		return (FORK_ERROR);
	return (SUCCESS);
}

static void		terminate_childs(const pid_t *pid, const size_t philo_nb)
{
	t_index i;

	i = 0;
	while (i < philo_nb)
	{
		kill(pid[i], SIGTERM);
		i++;
	}
}

int				main(int argc, char **argv)
{
	t_data			state;
	const t_params	parameters;
	t_error			error;

	if (argc < 5 || argc > 6)
		return (exit_error(USAGE, NULL));
	if (get_params((t_params*)&parameters, (const char **)argv))
		return (exit_error(ARGS_ERROR, NULL));
	if ((error = init_state(&state, &parameters)))
		return (exit_error(error, &state));
	if ((error = start_simulation(&state, &parameters)))
		return (exit_error(error, &state));
	sem_wait(state.death_sem);
	terminate_childs(state.pid, parameters.philo_nb);
	unlink_semaphores(parameters.philo_nb);
	return (EXIT_SUCCESS);
}
