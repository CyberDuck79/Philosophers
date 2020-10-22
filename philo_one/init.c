/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 12:13:04 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/22 12:30:18 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

t_error	        get_parameters(t_params *parameters, char **argv)
{
	if ((parameters->philo_nb = str_to_nb(argv[1])) < 2)
		return (ARGS_ERROR);
	if (parameters->philo_nb > 999)
		return (ARGS_ERROR);
	if ((parameters->tt_die = (t_time)str_to_nb(argv[2])) < 1)
		return (ARGS_ERROR);
	if ((parameters->tt_eat = (t_time)str_to_nb(argv[3])) < 1)
		return (ARGS_ERROR);
	parameters->tt_eat *= 1000;
	if ((parameters->tt_sleep = (t_time)str_to_nb(argv[4])) < 1)
		return (ARGS_ERROR);
	parameters->tt_sleep *= 1000;
	if (!argv[5])
		parameters->must_eat_nb = 0;
	else if ((parameters->must_eat_nb = str_to_nb(argv[5])) < 1)
		return (ARGS_ERROR);
	parameters->start_time = get_time();
	return (SUCCESS);
}

static t_error	init_mutexes(t_data *state, const t_params *parameters)
{
	t_index i;

	if (pthread_mutex_init(&state->write_mtx, NULL))
		return (MUTEX_ERROR);
	if (pthread_mutex_init(&state->death_mtx, NULL))
		return (MUTEX_ERROR);
	pthread_mutex_lock(&state->death_mtx);
	while (i < parameters->philo_nb)
	{
		if (pthread_mutex_init(&state->fork[i], NULL))
			return (MUTEX_ERROR);
		state->philo[i].fork[0] = &state->fork[i];
		state->philo[i].fork[1] = &state->fork[(i + 1) % parameters->philo_nb];
		state->philo[i].write_mtx = &state->write_mtx;
		state->philo[i].death_mtx = &state->death_mtx;
		if (pthread_mutex_init(&state->philo[i].launch_mtx, NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&state->philo[i].monit_mtx, NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&state->philo[i].eat_count_mtx, NULL))
			return (MUTEX_ERROR);
		pthread_mutex_lock(&state->philo[i].eat_count_mtx);
		i++;
	}
	return (SUCCESS);
}

t_error	        init_state(t_data *state, const t_params *parameters)
{
	t_index i;

    state->parameters = parameters;
	if (!(state->philo = \
		(t_philo*)malloc(sizeof(t_philo) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	if (!(state->fork = \
		(t_mutex*)malloc(sizeof(t_mutex) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	i = 0;
	while (i < parameters->philo_nb)
	{
		if ((state->philo[i].nb = nb_to_str(i + 1)) == NULL)
			return (MALLOC_ERROR);
		if (i + 1 < 10)
			state->philo[i].nb_len = 1;
		else if (i + 1 < 100)
			state->philo[i].nb_len = 2;
		else
			state->philo[i].nb_len = 3;
		state->philo[i].parameters = parameters;
		state->philo[i].launch_flag = NOT_STARTED;
		i++;
	}
	return (init_mutexes(state, parameters));
}
