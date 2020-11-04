/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 13:28:38 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/04 10:48:50 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

static t_error	launch_philo(t_data *state, const t_params *parameters)
{
	t_index		i;
	t_thread	tid;

	i = 0;
	while (i < parameters->philo_nb)
	{
		if (pthread_create(&tid, NULL, &philosopher, (void*)&state->philo[i]))
			return (THREAD_ERROR);
		pthread_detach(tid);
		pthread_mutex_lock(&state->philo[i].launch_mtx);
		while (state->philo[i].launch_flag == NOT_STARTED)
		{
			pthread_mutex_unlock(&state->philo[i].launch_mtx);
			pthread_mutex_lock(&state->philo[i].launch_mtx);
		}
		if (state->philo[i].launch_flag == ERROR)
			return (THREAD_ERROR);
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
	if (launch_philo(state, parameters))
		return (THREAD_ERROR);
	return (SUCCESS);
}

int				main(int argc, char **argv)
{
	t_data			state;
	const t_params	parameters;
	t_error			error;

	if (argc < 5 || argc > 6)
		return (exit_error(USAGE));
	if (get_params((t_params*)&parameters, (const char**)argv))
		return (exit_error(ARGS_ERROR));
	if ((error = init_state(&state, &parameters)))
		return (exit_error(error));
	if (start_simulation(&state, &parameters))
		return (exit_error(THREAD_ERROR));
	pthread_mutex_lock(&state.death_mtx);
	return (EXIT_SUCCESS);
}
