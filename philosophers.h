/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/20 13:05:56 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/16 20:55:23 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

# define ERROR -1
# define STDOUT 1
# define STDERR 2

# define FIRST 0
# define SECOND 1

typedef	int				t_flag;
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
	t_time				time_to_die;
	t_time				time_to_eat;
	t_time				time_to_sleep;
	int					must_eat_nb;
	t_time				start_time;
}						t_params;

typedef struct			s_print
{
	char				*nb;
	size_t				nb_len;
	char				**str;
	size_t				*str_len;
	t_mutex				*print_mtx;
}						t_print;

typedef struct			s_philo
{
	int					nb;
	t_mutex				*fork[2];
	t_mutex				*eating_mtx;
	t_time				last_eat_time;
	t_mutex				*started_mtx;
	t_flag				started;
	t_mutex				*at_table_mtx;
	const t_params		*params;
	t_print				print;
	t_thread			p_thread;
}						t_philo;

typedef struct			s_data
{
	t_mutex				*forks;
	t_mutex				*eating_mtx;
	t_mutex				*started_mtx;
	t_mutex				at_table_mtx;
	t_mutex				print_mtx;
	t_philo				*philo;
	char				*str[6];
	size_t				str_len[6];
	// new
	const t_params		parameters;
	t_mutex				end_sim_mtx;
}						t_data;

#endif