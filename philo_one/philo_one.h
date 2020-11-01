/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 15:39:44 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 10:43:01 by fhenrion         ###   ########.fr       */
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

typedef enum			e_launch_status
{
	NOT_STARTED,
	ERROR,
	STARTED
}						t_launch_status;

typedef enum			e_philo_state
{
	THINKING,
	TAKING,
	EATING,
	SLEEPING,
	DIED,
	DONE
}						t_philo_state;

typedef struct			s_params
{
	size_t				philo_nb;
	t_time				tt_die;
	t_time				tt_eat;
	t_time				tt_sleep;
	int					must_eat_nb;
	t_time				start_time;
}						t_params;

typedef struct			s_philo
{
	char				*nb;
	size_t				nb_len;
	t_mutex				*fork[2];
	t_mutex				*write_mtx;
	t_mutex				*death_mtx;
	t_launch_status		launch_flag;
	t_mutex				launch_mtx;
	t_mutex				eat_count_mtx;
	t_mutex				monit_mtx;
	t_time				death_time;
	const t_params		*parameters;
}						t_philo;

typedef struct			s_data
{
	t_mutex				*fork;
	t_philo				*philo;
	t_mutex				write_mtx;
	t_mutex				death_mtx;
	const t_params		*parameters;
}						t_data;

t_time					get_time(void);
int						update_time(t_time *last_time);
void					time_to_str(char *buf, t_time time);
int						str_to_nb(const char *str);
char					*nb_to_str(unsigned long nb);
size_t					nb_str_len(unsigned long nb);
int						exit_error(t_error error);
t_error					get_parameters(t_params *parameters, char **argv);
t_error					init_state(t_data *state, const t_params *parameters);
void					print_state(t_philo *philo, t_philo_state state);
void					*death_monitor(void *philo_void);
void					*philosopher(void *philo_void);
void					*eat_monitor(void *state_void);
int						left(t_index i, size_t len);
int						right(t_index i, size_t len);

#endif
