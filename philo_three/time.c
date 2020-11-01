/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 11:45:12 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 08:53:36 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_three.h"

t_time	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int		update_time(t_time *last_time)
{
	t_time		curr_time;

	curr_time = get_time();
	if (*last_time == curr_time)
		return (0);
	*last_time = curr_time;
	return (1);
}

void	time_to_str(char *buf, t_time time)
{
	t_index	i;

	i = 11;
	buf[i] = ' ';
	while (time)
	{
		i--;
		buf[i] = (time % 10) + '0';
		time /= 10;
	}
	while (i--)
		buf[i] = '0';
}
