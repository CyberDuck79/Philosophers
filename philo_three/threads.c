/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 12:38:15 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/07 20:56:39 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

static void	eating(t_philo *philo)
{
	sem_wait(philo->take_sem);
	sem_wait(philo->forks_sem);
	print_state(philo, TAKING);
	sem_wait(philo->forks_sem);
	print_state(philo, TAKING);
	sem_post(philo->take_sem);
	sem_wait(philo->monit_sem);
	philo->death_time = get_time() + philo->parameters->tt_die;
	sem_post(philo->monit_sem);
	print_state(philo, EATING);
	usleep(philo->parameters->tt_eat);
	sem_post(philo->eat_count_sem);
}

static void	sleeping(t_philo *philo)
{
	print_state(philo, SLEEPING);
	sem_post(philo->forks_sem);
	sem_post(philo->forks_sem);
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
			sem_wait(state->eat_count_sem);
			i++;
		}
	}
	print_state(&state->philo[0], DONE);
	sem_post(state->death_sem);
	return (NULL);
}

void		*death_monitor(void *philo_void)
{
	t_philo	*philo;

	philo = (t_philo*)philo_void;
	while (1)
	{
		sem_wait(philo->monit_sem);
		if (get_time() > philo->death_time)
		{
			print_state(philo, DIED);
			sem_post(philo->death_sem);
			return (NULL);
		}
		sem_post(philo->monit_sem);
		usleep(1000);
	}
	return (NULL);
}

void		philosopher(t_philo *philo)
{
	t_thread	tid;

	philo->death_time = get_time() + philo->parameters->tt_die;
	if (pthread_create(&tid, NULL, &death_monitor, (void*)philo))
	{
		write(STDERR, "monitor creation error\n", 24);
		sem_post(philo->launch_flag_sem);
		sem_post(philo->death_sem);
		sem_wait(philo->take_sem);
	}
	pthread_detach(tid);
	sem_post(philo->launch_flag_sem);
	sem_wait(philo->launch_barrier_sem);
	while (1)
	{
		eating(philo);
		sleeping(philo);
		print_state(philo, THINKING);
	}
}
