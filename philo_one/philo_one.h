/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/20 12:14:16 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/20 13:16:32 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_ONE_H
# define PHILO_ONE_H

# include <sys/time.h>
# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>

# define STDOUT 1
# define STDERR 2

typedef unsigned long	t_time;
typedef unsigned long	t_index;
typedef pthread_mutex_t	t_mutex;
typedef pthread_t		t_thread;

typedef enum			e_error
{
	SUCCESS,
	USAGE,
	ARGS_ERROR,
	MALLOC_ERROR,
	MUTEX_ERROR,
	THREAD_ERROR
}						t_error;

typedef enum			e_state
{
	THINKING,
	TAKING,
	EATING,
	SLEEPING,
	DIED,
	DONE
}						t_state;

typedef struct			s_params
{
	int					philo_nb;
	t_time				tt_die;
	t_time				tt_eat;
	t_time				tt_sleep;
	int					must_eat_nb;
	t_mutex				write_mtx;
	t_mutex				death_mtx;
}						t_params;

typedef struct			s_philo
{
	char				*nb;
	size_t				nb_len;
	t_mutex				*fork[2];
	t_mutex				eat_mtx;
	t_mutex				monit_mtx;
	t_time				death_time;
	const t_params		*params;
}						t_philo;

typedef struct			s_data
{
	t_mutex				*forks;
	t_philo				*philo;
	const t_params		parameters;
}						t_data;

#endif