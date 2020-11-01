/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 15:39:58 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 08:45:24 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

static char		*get_state_str(t_philo_state state)
{
	if (state == EATING)
		return (" is eating\n");
	else if (state == SLEEPING)
		return (" is sleeping\n");
	else if (state == TAKING)
		return (" has taken a fork\n");
	else if (state == THINKING)
		return (" is thinking\n");
	else if (state == DONE)
		return ("must eat count reached\n");
	return (" died\n");
}

static size_t	get_state_len(t_philo_state state)
{
	if (state == EATING)
		return (12);
	else if (state == SLEEPING)
		return (14);
	else if (state == TAKING)
		return (19);
	else if (state == THINKING)
		return (14);
	else if (state == DONE)
		return (24);
	return (7);
}

int			print_state(t_philo *philo, t_philo_state state)
{
	static int		sim_end = 0;
	static t_time	time = 0;
	static char		time_str[12];

	sem_wait(philo->write_sem);
	if (!sim_end)
	{
		if (state == DIED || state == DONE)
			sim_end = 1;
		if (update_time(&time))
			time_to_str(time_str, time - philo->parameters->start_time);
		write(STDOUT, time_str, 12);
		if (state != DONE)
			write(STDOUT, philo->nb, philo->nb_len);
		write(STDOUT, get_state_str(state), get_state_len(state));
		sem_post(philo->write_sem);
	}
	return (0);
}
