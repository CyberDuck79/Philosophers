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
time_t			get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int				update_time(time_t *last_time)
{
	time_t			curr_time;

	curr_time = get_time();
	if (*last_time == curr_time)
		return (0);
	*last_time = curr_time;
	return (1);
}

void			time_to_str(char *buf, t_time time)
{
	t_index	i;

	i = 14;
	buf[i] = ' ';
	while (time)
	{
		i--;
		buf[i] = (time % 10) + '0';
		time /= 10;
	}
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
	return (SUCCESS);
}

t_error	init_mutexes(t_data *simulation, const t_params *parameters)
{
	t_index i;

	if (pthread_mutex_init((t_mutex*)&parameters->write_mtx, NULL))
		return (MUTEX_ERROR);
	if (pthread_mutex_init((t_mutex*)&parameters->death_mtx, NULL))
		return (MUTEX_ERROR);
	pthread_mutex_lock((t_mutex*)&parameters->death_mtx);
	while (i < parameters->philo_nb)
	{
		simulation->philo[i].fork[0] = &simulation->forks[i];
		simulation->philo[i].fork[1] = \
		&simulation->forks[(i + 1) % parameters->philo_nb];
		if (pthread_mutex_init(&simulation->philo[i].monit_mtx, NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&simulation->philo[i].eat_mtx, NULL))
			return (MUTEX_ERROR);
		pthread_mutex_lock(&simulation->philo[i].eat_mtx);
		i++;
	}
	return (SUCCESS);
}

t_error	init_simulation(t_data *simulation, const t_params *parameters)
{
	t_index i;

	if (!(simulation->philo = 
		(t_philo*)malloc(sizeof(t_philo) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	if (!(simulation->forks = 
		(t_mutex*)malloc(sizeof(t_mutex) * parameters->philo_nb)))
		return (MALLOC_ERROR);
	i = 0;
	while (i < parameters->philo_nb)
	{
		if ((simulation->philo[i].nb = nb_to_str(i + 1)) == NULL)
			return (MALLOC_ERROR);
		if (i + 1 < 10)
			simulation->philo[i].nb_len = 1;
		else if (i + 1 < 100)
			simulation->philo[i].nb_len = 2;
		else
			simulation->philo[i].nb_len = 3;
		simulation->philo[i].params = parameters;
		i++;
	}
	return (init_mutexes(simulation, parameters));
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
	static time_t	time = 0;
	static char		time_str[15];

	pthread_mutex_lock((t_mutex*)&philo->params->write_mtx);
	if (!sim_end)
	{
		if (update_time(&time))
			time_to_str(time_str, time);
		write(STDOUT, time_str, 15);
		if (state != DONE)
			write(STDOUT, philo->nb, philo->nb_len);
		write(STDOUT, get_state_str(state), get_state_len(state));
		if (state == DIED)
			sim_end = 1;
	}
	pthread_mutex_unlock((t_mutex*)&philo->params->write_mtx);
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

/* start simulation.c */
void	*eat_monitor(void *sim_void)
{
	t_index	i;
	t_data	*simulation;
	int		eat_count;

	simulation = (t_data*)sim_void;
	eat_count = simulation->parameters.must_eat_nb;
	while (eat_count--)
	{
		i = 0;
		while (i < simulation->parameters.philo_nb)
		{
			pthread_mutex_lock(&simulation->philo[i].eat_mtx);
			i++;
		}
	}
	print_state(&simulation->philo[0], DONE);
	pthread_mutex_unlock((t_mutex*)&simulation->parameters.death_mtx);
	return (NULL);
}

void	*death_monitor(void *philo_void)
{
	t_philo	*philo;

	philo = (t_philo*)philo_void;
	while (1)
	{
		pthread_mutex_lock(&philo->monit_mtx);
		if (get_time() > philo->death_time) // is eating utile ???
		{
			print_state(philo, DIED);
			pthread_mutex_unlock(&philo->monit_mtx);
			pthread_mutex_unlock((t_mutex*)&philo->params->death_mtx);
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
	while (1)
	{
		eating(philo);
		sleeping(philo);
		print_state(philo, THINKING);
	}
	return (NULL);
}

t_error	start_simulation(t_data *simulation, const t_params *parameters)
{
	t_index		i;
	t_thread	tid;

	if (parameters->must_eat_nb > 0)
	{
		if (pthread_create(&tid, NULL, &eat_monitor, (void*)simulation))
			return (THREAD_ERROR);
		pthread_detach(tid);
	}
	i = 0;
	while (i < parameters->philo_nb)
	{
		if (pthread_create(&tid, NULL, &philosophing, \
		(void*)&simulation->philo[i]))
			return (THREAD_ERROR);
		pthread_detach(tid);
		usleep(100);
		i++;
	}
	return (SUCCESS);
}
/* end simulation.c */

//*****************************************************************************

/* start main.c */
int		 main(int argc, char **argv)
{
	t_data	simulation;
	t_error	error;

	if (argc < 5 || argc > 6)
		return (exit_error(USAGE));
	if (get_parameters((t_params*)&simulation.parameters, argv))
		return (exit_error(ARGS_ERROR));
	if ((error = init_simulation(&simulation, &simulation.parameters)))
		return (exit_error(error));
	if (start_simulation(&simulation, &simulation.parameters))
		return (exit_error(THREAD_ERROR));
	pthread_mutex_lock((t_mutex*)&simulation.parameters.death_mtx);
	pthread_mutex_unlock((t_mutex*)&simulation.parameters.death_mtx);
	return (EXIT_SUCCESS);
}
/* end main.c */
