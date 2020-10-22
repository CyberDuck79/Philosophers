/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 12:10:10 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/22 12:10:35 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

int     str_to_nb(const char *str)
{
	int		nb;
	size_t	nb_len;
	int		mult;

	if (*str == '-')
		return (-1);
	if (*str == '+')
		str++;
	nb = 0;
	nb_len = 0;
	while (str[nb_len] >= '0' && str[nb_len] <= '9')
		nb_len++;
	while (nb_len--)
	{
		mult = (nb_len ? 10 * nb_len : 1);
		nb += (*str - '0') * mult;
		str++;
	}
	return (nb);
}

char    *nb_to_str(unsigned long nb)
{
	char			*new;
	unsigned long	tmp;
	size_t			len;

	len = 1;
	tmp = nb;
	while (tmp /= 10)
		len++;
	if (!(new = (char*)malloc(sizeof(char) * (len + 1))))
		return (NULL);
	new[0] = '0';
	new[len] = '\0';
	while (nb)
	{
		len--;
		new[len] = (nb % 10) + '0';
		nb /= 10;
	}
	return (new);
}

int     exit_error(t_error error)
{
	if (error == USAGE)
		write(STDERR, "USAGE : philo_one number_of_philosopher time_to_die"
		" time_to_eat time_to_sleep "
		"[number_of_time_each_philoophers_must_eat]\n", 121);
	else if (error == ARGS_ERROR)
		write(STDERR, "Invalid argument\n", 17);
	else if (error == MALLOC_ERROR)
		write(STDERR, "Memoy allocation error\n", 23);
	else if (error == MUTEX_ERROR)
		write(STDERR, "Mutex creation error\n", 21);
	else if (error == THREAD_ERROR)
		write(STDERR, "Thread creation error\n", 22);
	else
		write(STDERR, "Unimplemented error ?\n", 22);
	return ((int)error);
}
