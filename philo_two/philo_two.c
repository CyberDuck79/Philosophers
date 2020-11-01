/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_two.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 13:28:38 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/31 18:13:45 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

static t_error	launch_philo(t_data *state, const size_t philo_nb)
{
	t_index 	i;
	t_thread	tid;

	i = 0;
	while (i < philo_nb)
	{
		if (pthread_create(&tid, NULL, &philosopher, (void*)&state->philo[i]))
			return (THREAD_ERROR);
		pthread_detach(tid);
		sem_wait(state->philo[i].launch_flag_sem);
		while (state->philo[i].launch_flag == NOT_READY)
		{
			sem_post(state->philo[i].launch_flag_sem);
			sem_wait(state->philo[i].launch_flag_sem);
		}
		sem_post(state->philo[i].launch_flag_sem);
		if (state->philo[i].launch_flag == ERROR)
			return (THREAD_ERROR);
		i++;
	}
	return (SUCCESS);
}

static t_error	start_simulation(t_data *state, const t_params *parameters)
{
	t_thread	tid;
	t_index		i;

	if (parameters->must_eat_nb > 0)
	{
		if (pthread_create(&tid, NULL, &eat_monitor, (void*)state))
			return (THREAD_ERROR);
		pthread_detach(tid);
	}
	if (launch_philo(state, parameters->philo_nb))
		return (THREAD_ERROR);
	i = 0;
	while (i < parameters->philo_nb)
	{
		sem_post(state->launch_barrier_sem);
		i++;
	}
	return (SUCCESS);
}

int		 main(int argc, char **argv)
{
	t_data			state;
	const t_params	parameters;
	t_error			error;

	if (argc < 5 || argc > 6)
		return (exit_error(USAGE, NULL));
	if (get_parameters((t_params*)&parameters, argv))
		return (exit_error(ARGS_ERROR, NULL));
	if ((error = init_state(&state, &parameters)))
		return (exit_error(error, &state));
	if (start_simulation(&state, &parameters))
		return (exit_error(THREAD_ERROR, &state));
	sem_wait(state.death_sem);
	unlink_semaphores(parameters.philo_nb);
	//system("leaks philo_two");
	return (EXIT_SUCCESS);
}
