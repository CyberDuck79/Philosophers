/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 12:38:15 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/23 16:09:31 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

static void	eating(t_philo *philo)
{
	pthread_mutex_lock(philo->fork[0]);
	print_state(philo, TAKING);
	pthread_mutex_lock(philo->fork[1]);
	print_state(philo, TAKING);
	pthread_mutex_lock(&philo->monit_mtx);
	philo->death_time = get_time() + philo->parameters->tt_die;
	pthread_mutex_unlock(&philo->monit_mtx);
	print_state(philo, EATING);
	usleep(philo->parameters->tt_eat);
	pthread_mutex_unlock(&philo->eat_count_mtx);
}

static void	sleeping(t_philo *philo)
{
	print_state(philo, SLEEPING);
	pthread_mutex_unlock(philo->fork[1]);
	pthread_mutex_unlock(philo->fork[0]);
	usleep(philo->parameters->tt_sleep);
}

void		*eat_monitor(void *state_void)
{
	t_index	i;
	t_data	*state;
	int		eat_count;

	state = (t_data*)state_void;
	eat_count = state->parameters->must_eat_nb;
	while (eat_count--)
	{
		i = 0;
		while (i < state->parameters->philo_nb)
		{
			pthread_mutex_lock(&state->philo[i].eat_count_mtx);
			i++;
		}
	}
	print_state(&state->philo[0], DONE);
	pthread_mutex_unlock(&state->death_mtx);
	return (NULL);
}

void		*death_monitor(void *philo_void)
{
	t_philo	*philo;

	philo = (t_philo*)philo_void;
	while (1)
	{
		pthread_mutex_lock(&philo->monit_mtx);
		if (get_time() > philo->death_time)
		{
			print_state(philo, DIED);
			pthread_mutex_unlock(philo->death_mtx);
			return (NULL);
		}
		pthread_mutex_unlock(&philo->monit_mtx);
		usleep(1000);
	}
	return (NULL);
}

void		*philosopher(void *philo_void)
{
	t_philo		*philo;
	t_thread	tid;

	philo = (t_philo*)philo_void;
	philo->death_time = get_time() + philo->parameters->tt_die;
	if (pthread_create(&tid, NULL, &death_monitor, philo_void))
	{
		pthread_mutex_lock(&philo->launch_mtx);
		philo->launch_flag = ERROR;
		pthread_mutex_unlock(&philo->launch_mtx);
		return (NULL);
	}
	pthread_detach(tid);
	pthread_mutex_lock(&philo->launch_mtx);
	philo->launch_flag = STARTED;
	pthread_mutex_unlock(&philo->launch_mtx);
	while (1)
	{
		eating(philo);
		sleeping(philo);
		print_state(philo, THINKING);
	}
	return (NULL);
}
