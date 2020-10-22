/* *********************************************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   philo_one.c										:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: fhenrion <fhenrion@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2020/10/16 20:22:50 by fhenrion		  #+#	#+#			 */
/*   Updated: 2020/10/16 22:29:45 by fhenrion		 ###   ########.fr	   */
/*																			*/
/* *********************************************************************************************************** */

#include "philo_one.h"

/* start time.c */
t_time			get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int				update_time(t_time *last_time)
{
	t_time		curr_time;

	curr_time = get_time();
	if (*last_time == curr_time)
		return (0);
	*last_time = curr_time;
	return (1);
}

#include <stdio.h>
void			time_to_str(char *buf, t_time time)
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
/* end time.c */

//*****************************************************************************

/* start utils.c */
char			*nb_to_str(unsigned long nb)
{
	char	*new;
	t_index	i;
	size_t	len;

	len = 2;
	i = nb;
	while (i /= 10)
		len++;
	if ((new = malloc(sizeof(char) * len)) == NULL)
		return (NULL);
	len--;
	new[len] = '\0';
	if (!nb)
		new[0] = '0';
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
	if (error == ARGS_ERROR)
		write(STDERR, "Invalid argument\n", 17);
	if (error == MALLOC_ERROR)
		write(STDERR, "Memoy allocation error\n", 23);
	if (error == MUTEX_ERROR)
		write(STDERR, "Mutex creation error\n", 21);
	if (error == THREAD_ERROR)
		write(STDERR, "Thread creation error\n", 22);
	return ((int)error);
}

/* end utils.c */

//*****************************************************************************

/* start init.c */
t_error	get_parameters(t_params *parameters, char **argv)
{
	if ((parameters->philo_nb = atoi(argv[1])) < 2)
		return (ARGS_ERROR);
	if (parameters->philo_nb > 999)
		return (ARGS_ERROR);
	if ((parameters->tt_die = atoi(argv[2])) < 1)
		return (ARGS_ERROR);
	if ((parameters->tt_eat = atoi(argv[3])) < 1)
		return (ARGS_ERROR);
	parameters->tt_eat *= 1000;
	if ((parameters->tt_sleep = atoi(argv[4])) < 1)
		return (ARGS_ERROR);
	parameters->tt_sleep *= 1000;
	if (!argv[5])
		parameters->must_eat_nb = 0;
	else if ((parameters->must_eat_nb = atoi(argv[5])) < 1)
		return (ARGS_ERROR);
	parameters->start_time = get_time();
	return (SUCCESS);
}

t_error	init_mutexes(t_data *state, const t_params *parameters)
{
	t_index i;

	if (pthread_mutex_init(&state->write_mtx, NULL))
		return (MUTEX_ERROR);
	if (pthread_mutex_init(&state->death_mtx, NULL))
		return (MUTEX_ERROR);
	pthread_mutex_lock(&state->death_mtx);
	while (i < parameters->philo_nb)
	{
		if (pthread_mutex_init(&state->forks[i], NULL))
			return (MUTEX_ERROR);
		state->philo[i].fork[0] = &state->forks[i];
		state->philo[i].fork[1] = &state->forks[(i + 1) % parameters->philo_nb];
		state->philo[i].write_mtx = &state->write_mtx;
		state->philo[i].death_mtx = &state->death_mtx;
		if (pthread_mutex_init(&state->philo[i].launch_mtx, NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&state->philo[i].monit_mtx, NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&state->philo[i].eat_mtx, NULL))
			return (MUTEX_ERROR);
		pthread_mutex_lock(&state->philo[i].eat_mtx);
		i++;
	}
	return (SUCCESS);
}

t_error	init_state(t_data *state, const t_params *parameters)
{
	t_index i;

	if (!(state->philo = 
		(t_philo*)malloc(sizeof(t_philo) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	if (!(state->forks = 
		(t_mutex*)malloc(sizeof(t_mutex) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	i = 0;
	while (i < parameters->philo_nb)
	{
		if ((state->philo[i].nb = nb_to_str(i + 1)) == NULL)
			return (MALLOC_ERROR);
		if (i + 1 < 10)
			state->philo[i].nb_len = 1;
		else if (i + 1 < 100)
			state->philo[i].nb_len = 2;
		else
			state->philo[i].nb_len = 3;
		state->philo[i].params = parameters;
		state->philo[i].launch_flag = 0;
		i++;
	}
	return (init_mutexes(state, parameters));
}
/* end init.c */

//*****************************************************************************

/* start print.c */
static char	*get_state_str(t_state state)
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

static size_t	get_state_len(t_state state)
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

void	print_state(t_philo *philo, t_state state)
{
	static int		sim_end = 0;
	static t_time	time = 0;
	static char		time_str[12];

	pthread_mutex_lock(philo->write_mtx);
	if (!sim_end)
	{
		if (state == DIED || state == DONE)
			sim_end = 1;
		if (update_time(&time))
			time_to_str(time_str, time - philo->params->start_time);
		write(STDOUT, time_str, 12);
		if (state != DONE)
			write(STDOUT, philo->nb, philo->nb_len);
		write(STDOUT, get_state_str(state), get_state_len(state));
		pthread_mutex_unlock(philo->write_mtx);
	}
}
/* end print.c */

//*****************************************************************************

/* start action.c */
void	eating(t_philo *philo)
{
	pthread_mutex_lock(philo->fork[0]);
	print_state(philo, TAKING);
	pthread_mutex_lock(philo->fork[1]);
	print_state(philo, TAKING);
	pthread_mutex_lock(&philo->monit_mtx);
	philo->death_time = get_time() + philo->params->tt_die;
	print_state(philo, EATING);
	usleep(philo->params->tt_eat);
	pthread_mutex_unlock(&philo->monit_mtx);
	pthread_mutex_unlock(&philo->eat_mtx);
}


void	sleeping(t_philo *philo)
{
	print_state(philo, SLEEPING);
	pthread_mutex_unlock(philo->fork[0]);
	pthread_mutex_unlock(philo->fork[1]);
	usleep(philo->params->tt_sleep);
}
/* end action */

//*****************************************************************************

/* start state.c */
void	*eat_monitor(void *sim_void)
{
	t_index	i;
	t_data	*state;
	int		eat_count;

	state = (t_data*)sim_void;
	eat_count = state->parameters.must_eat_nb;
	while (eat_count--)
	{
		i = 0;
		while (i < state->parameters.philo_nb)
		{
			pthread_mutex_lock(&state->philo[i].eat_mtx);
			i++;
		}
	}
	print_state(&state->philo[0], DONE);
	pthread_mutex_unlock(&state->death_mtx);
	return (NULL);
}

void	*death_monitor(void *philo_void)
{
	t_philo	*philo;

	philo = (t_philo*)philo_void;
	while (1)
	{
		pthread_mutex_lock(&philo->monit_mtx);
		if (get_time() > philo->death_time)
		{
			print_state(philo, DIED);
			pthread_mutex_unlock(philo->death_mtx);
			return (NULL);
		}
		pthread_mutex_unlock(&philo->monit_mtx);
		usleep(1000);
	}
	return (NULL);
}

void	*philosophing(void *philo_void)
{
	t_philo		*philo;
	t_thread	tid;

	philo = (t_philo*)philo_void;
	philo->death_time = get_time() + philo->params->tt_die;
	if (pthread_create(&tid, NULL, &death_monitor, philo_void))
	{
		exit_error(THREAD_ERROR);
		return (NULL);
	}
	pthread_detach(tid);
	pthread_mutex_lock(&philo->launch_mtx);
	philo->launch_flag = 1;
	pthread_mutex_unlock(&philo->launch_mtx);
	while (1)
	{
		eating(philo);
		sleeping(philo);
		print_state(philo, THINKING);
	}
	return (NULL);
}

t_error	launch_philo(t_data *state, const t_params *parameters, t_index i)
{
	t_thread	tid;

	while (i < parameters->philo_nb)
	{
		if (pthread_create(&tid, NULL, &philosophing, \
		(void*)&state->philo[i]))
			return (THREAD_ERROR);
		pthread_detach(tid);
		pthread_mutex_lock(&state->philo[i].launch_mtx);
		while (!state->philo[i].launch_flag)
		{
			pthread_mutex_unlock(&state->philo[i].launch_mtx);
			pthread_mutex_lock(&state->philo[i].launch_mtx);
		}
		i += 2;
	}
	return (SUCCESS);
}

t_error	start_state(t_data *state, const t_params *parameters)
{
	t_thread	tid;

	if (parameters->must_eat_nb > 0)
	{
		if (pthread_create(&tid, NULL, &eat_monitor, (void*)state))
			return (THREAD_ERROR);
		pthread_detach(tid);
	}
	launch_philo(state, parameters, 0);
	launch_philo(state, parameters, 1);
	return (SUCCESS);
}

int		 main(int argc, char **argv)
{
	t_data	state;
	t_error	error;

	if (argc < 5 || argc > 6)
		return (exit_error(USAGE));
	if (get_parameters((t_params*)&state.parameters, argv))
		return (exit_error(ARGS_ERROR));
	if ((error = init_state(&state, &state.parameters)))
		return (exit_error(error));
	if (start_state(&state, &state.parameters))
		return (exit_error(THREAD_ERROR));
	pthread_mutex_lock(&state.death_mtx);
	return (EXIT_SUCCESS);
}
/* end state.c */
