/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 12:10:10 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/23 12:44:53 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

static int	ft_pow(int op1, int op2)
{
	int mult = op1;

	if (!op2)
		return (1);
	if (op2 == 1)
		return (op1);
	op2--;
	while (op2--)
		op1 *= mult;
	return (op1);
}

int			str_to_nb(const char *str)
{
	int		nb;
	int		nb_len;

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
		nb += (*str - '0') * ft_pow(10, nb_len);
		str++;
	}
	return (nb);
}

size_t		nb_str_len(unsigned long nb)
{
	if (nb < 10)
		return (1);
	if (nb < 100)
		return (2);
	if (nb < 1000)
		return (3);
	return (4);
}

char		*nb_to_str(unsigned long nb)
{
	char			*new;
	size_t			len;

	len = nb_str_len(nb);
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

int			exit_error(t_error error)
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
