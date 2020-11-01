/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/31 15:04:14 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/31 17:04:07 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_two.h"

void			unlink_semaphores(const size_t philo_nb)
{
	t_index	i;
	char 	*name;

	sem_unlink("/forks");
	sem_unlink("/take");
	sem_unlink("/launch_barrier");
	sem_unlink("/write");
	sem_unlink("/death");
	sem_unlink("/eat_count");
	i = 0;
	while (i < philo_nb)
	{
		if ((name = gen_name("/launch_", i)))
		{
			sem_unlink(name);
			free(name);
		}
		if ((name = gen_name("/monit_", i)))
		{
			sem_unlink(name);
			free(name);
		}
		i++;
	}
}

static size_t	str_len(const char *str)
{
	size_t len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

char			*gen_name(const char *prefix, t_index i)
{
	char 	*name_str;
	char	*nb_str;

	if (!(name_str = (char*)malloc(str_len(prefix) + nb_str_len(i) + 1)))
		return (NULL);
	if (!(nb_str = nb_to_str(i)))
		return (NULL);
	str_copy(name_str, prefix);
	str_copy(name_str + str_len(prefix), nb_str);
	free(nb_str);
	return (name_str);
}
