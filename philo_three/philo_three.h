/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_three.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 15:39:44 by fhenrion          #+#    #+#             */
/*   Updated: 2020/11/01 23:05:04 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_THREE_H
# define PHILO_THREE_H

# include <sys/time.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <fcntl.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <unistd.h>
# include <stdlib.h>

# define STDOUT 1
# define STDERR 2

typedef unsigned long	t_time;
typedef unsigned long	t_index;
typedef sem_t			t_semaphore;
typedef pthread_t		t_thread;

typedef enum			e_error
{
	SUCCESS,
	USAGE,
	ARGS_ERROR,
	MALLOC_ERROR,
	SEMAPHORE_ERROR,
	THREAD_ERROR,
	FORK_ERROR
}						t_error;

typedef enum			e_launch_status
{
	NOT_READY,
	ERROR,
	READY
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
	t_semaphore			*forks_sem;
	t_semaphore			*take_sem;
	t_semaphore			*write_sem;
	t_semaphore			*death_sem;
	t_semaphore			*launch_barrier_sem;
	t_semaphore			*launch_flag_sem;
	const char			*launch_name;
	t_launch_status		launch_flag;
	t_semaphore			*eat_count_sem;
	t_semaphore			*monit_sem;
	const char			*monit_name;
	t_time				death_time;
	const t_params		*parameters;
}						t_philo;

typedef struct			s_data
{
	pid_t				*pid;
	t_semaphore			*forks_sem;
	t_semaphore			*take_sem;
	t_philo				*philo;
	t_semaphore			*write_sem;
	t_semaphore			*death_sem;
	t_semaphore			*launch_barrier_sem;
	t_semaphore			*eat_count_sem;
	const t_params		*parameters;
}						t_data;

t_time					get_time(void);
int						update_time(t_time *last_time);
void					time_to_str(char *buf, t_time time);
int						str_to_nb(const char *str);
char					*nb_to_str(unsigned long nb);
size_t					nb_str_len(const unsigned long nb);
void					str_copy(char *dest, const char *src);
int						exit_error(const t_error error, t_data *state);
t_error					get_params(t_params *parameters, const char **argv);
t_error					init_state(t_data *state, const t_params *parameters);
int						print_state(t_philo *philo, const t_philo_state state);
void					*death_monitor(void *philo_void);
void					philosopher(t_philo *philo);
void					*eat_monitor(void *state_void);
void					unlink_semaphores(const size_t philo_nb);
char					*gen_name(const char *prefix, t_index i);

#endif
