/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/23 16:11:10 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 22:56:20 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_one.h"

int		left(const t_index i, const size_t len)
{
	return (((i) + (len - 1)) % len);
}

int		right(const t_index i, const size_t len)
{
	return (((i) + 1) % len);
}
