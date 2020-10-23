/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 15:38:57 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/23 18:14:24 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

t_error			get_parameters(t_params *parameters, char **argv)
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

static t_error	init_global_mutexes(t_data *state)
{
	if (pthread_mutex_init(&state->write_mtx, NULL))
		return (MUTEX_ERROR);
	if (pthread_mutex_init(&state->death_mtx, NULL))
		return (MUTEX_ERROR);
	pthread_mutex_lock(&state->death_mtx);
	return (SUCCESS);
}

static t_error	init_mutexes(t_data *state, const t_params *parameters)
{
	t_index i;

	if (init_global_mutexes(state))
		return (MUTEX_ERROR);
	i = 0;
	while (i < parameters->philo_nb)
	{
		if (pthread_mutex_init(&state->fork[i], NULL))
			return (MUTEX_ERROR);
		state->philo[i].fork[0] = &state->fork[left(i, parameters->philo_nb)];
		state->philo[i].fork[1] = &state->fork[right(i, parameters->philo_nb)];
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

t_error			init_state(t_data *state, const t_params *parameters)
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
		if (!(state->philo[i].nb = nb_to_str(i + 1)))
			return (MALLOC_ERROR);
		state->philo[i].nb_len = nb_str_len(i + 1);
		state->philo[i].parameters = parameters;
		state->philo[i].launch_flag = NOT_STARTED;
		i++;
	}
	return (init_mutexes(state, parameters));
}
