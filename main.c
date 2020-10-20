/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/20 13:18:58 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/16 16:10:30 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"
/* for tests */
#include "string.h"

int				g_at_table;
t_mutex			g_at_table_mtx;
t_mutex			g_end_mtx;

time_t			get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int				update_time(time_t *last_time)
{
	struct timeval	tv;
	time_t			curr_time;

	gettimeofday(&tv, NULL);
	curr_time = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	if (*last_time == curr_time)
		return (0);
	*last_time = curr_time;
	return (1);
}

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

void			print_state(t_print *print, t_state state)
{
	static time_t	time;
	static char		time_str[15];

	pthread_mutex_lock(print->print_mtx);
	pthread_mutex_lock(&g_at_table_mtx);
	if (!g_at_table)
		pthread_mutex_lock(&g_end_mtx);
	pthread_mutex_unlock(&g_at_table_mtx);
	if (update_time(&time))
		time_to_str(time_str, time);
	write(STDOUT, time_str, 15);
	write(STDOUT, print->nb, print->nb_len);
	write(STDOUT, print->str[state], print->str_len[state]);
	pthread_mutex_unlock(print->print_mtx);
}

void			take_forks_and_eat(t_philo *philo)
{
	pthread_mutex_lock(philo->fork[FIRST]);
	print_state(&philo->print, TAKING);
	pthread_mutex_lock(philo->fork[SECOND]);
	print_state(&philo->print, TAKING);
	print_state(&philo->print, EATING);
	pthread_mutex_lock(philo->eating_mtx);
	philo->last_eat_time = get_time();
	pthread_mutex_unlock(philo->eating_mtx);
	usleep(philo->params->time_to_eat);
	pthread_mutex_unlock(philo->fork[FIRST]);
	pthread_mutex_unlock(philo->fork[SECOND]);
}

void			*philosophing(void *data)
{
	t_philo	*philo;
	int		meal;

	philo = (t_philo*)data;
	pthread_mutex_lock(philo->started_mtx);
	philo->started = 1;
	pthread_mutex_unlock(philo->started_mtx);
	meal = 0;
	while (1)
	{
		take_forks_and_eat(philo);
		if (++meal == philo->params->must_eat_nb)
		{
			print_state(&philo->print, DONE);
			pthread_mutex_lock(&g_at_table_mtx);
			if (g_at_table)
				g_at_table--;
			pthread_mutex_unlock(&g_at_table_mtx);
			break;
		}
		print_state(&philo->print, SLEEPING);
		usleep(philo->params->time_to_sleep);
		print_state(&philo->print, THINKING);
	}
	pthread_mutex_lock(&g_end_mtx);
	return (NULL);
}


/*
// one monit thread per philo thread
void			*monitoring(void *data)
{
	t_philo	*philo;

	philo = (t_philo*)data;
	while (1)
	{
		pthread_mutex_lock(philo->eating_mtx);
		if (get_time() - philo->last_eat_time > philo->params->time_to_die)
		{
			print_state(&philo->print, DIED);
			pthread_mutex_lock(philo->at_table_mtx);
			g_at_table = 0;
			pthread_mutex_unlock(philo->at_table_mtx);
			break;
		}
		pthread_mutex_unlock(philo->eating_mtx);
		usleep(1);
	}
	return (NULL);
}
*/

// one monit thread for all philo threads
void			*monitoring(void *data)
{
	t_philo	*p;
	t_index	i;

	p = (t_philo*)data;
	while (1)
	{
		i = 0;
		while (i < p->params->philo_nb)
		{
			pthread_mutex_lock(p[i].eating_mtx);
			if (get_time() - p[i].last_eat_time > p->params->time_to_die)
			{
				print_state(&p[i].print, DIED);
				pthread_mutex_lock(&g_at_table_mtx);
				g_at_table = 0;
				pthread_mutex_unlock(&g_at_table_mtx);
				pthread_mutex_lock(&g_end_mtx);
				return (NULL);
			}
			pthread_mutex_unlock(p[i].eating_mtx);
			i++;
		}
		usleep(100);
	}
	return (NULL);
}

t_error			launch_threads(t_philo *philo, int philo_nb, t_index i)
{
	while (i < philo_nb)
	{
		if (pthread_create(&philo[i].p_thread, NULL, &philosophing, &philo[i]))
			return (THREAD_ERROR);
		pthread_detach(philo[i].p_thread);
		pthread_mutex_lock(philo[i].started_mtx);
		while (!philo[i].started)
		{
			pthread_mutex_unlock(philo[i].started_mtx);
			pthread_mutex_lock(philo[i].started_mtx);
		}
		i += 2;
	}
	return (SUCCESS);
}

t_error			launch_simulation(int philo_nb, t_philo *philo)
{
	t_thread	monit_thread;

	if (launch_threads(philo, philo_nb, 0))
		return (THREAD_ERROR);
	if (launch_threads(philo, philo_nb, 1))
		return (THREAD_ERROR);
	if (pthread_create(&monit_thread, NULL, &monitoring, philo))
		return (THREAD_ERROR);
	pthread_detach(monit_thread);
	return (SUCCESS);
}

t_error			philo_init(int philo_nb, t_data *data, const t_params *params)
{
	t_index	i;

	i = 0;
	while (i < philo_nb)
	{
		data->philo[i].fork[i % 2 ? SECOND : FIRST] = &data->forks[i];
		data->philo[i].fork[i % 2 ? FIRST : SECOND] = \
		&data->forks[(i + 1) % philo_nb];
		data->philo[i].eating_mtx = &data->eating_mtx[i];
		data->philo[i].last_eat_time = params->start_time;
		data->philo[i].started_mtx = &data->started_mtx[i];
		data->philo[i].started = 0;
		data->philo[i].params = params;
		if ((data->philo[i].print.nb = nb_to_str(i + 1)) == NULL)
			return (MALLOC_ERROR);
		data->philo[i].print.nb_len = strlen(data->philo[i].print.nb);
		data->philo[i].print.str = data->str;
		data->philo[i].print.str_len = data->str_len;
		data->philo[i].print.print_mtx = &data->print_mtx;
		i++;
	}
	return (SUCCESS);
}

t_error			str_init(char *str[6], size_t str_len[6])
{
	if ((str[THINKING] = strdup(" is thinking\n")) == NULL)
		return (MALLOC_ERROR);
	str_len[THINKING] = strlen(str[THINKING]);
	if ((str[TAKING] = strdup(" has taken a fork\n")) == NULL)
		return (MALLOC_ERROR);
	str_len[TAKING] = strlen(str[TAKING]);
	if ((str[EATING] = strdup(" is eating\n")) == NULL)
		return (MALLOC_ERROR);
	str_len[EATING] = strlen(str[EATING]);
	if ((str[SLEEPING] = strdup(" is sleeping\n")) == NULL)
		return (MALLOC_ERROR);
	str_len[SLEEPING] = strlen(str[SLEEPING]);
	if ((str[DIED] = strdup(" died\n")) == NULL)
		return (MALLOC_ERROR);
	str_len[DIED] = strlen(str[DIED]);
	if ((str[DONE] = strdup(" has eated enought\n")) == NULL)
		return (MALLOC_ERROR);
	str_len[DONE] = strlen(str[DONE]);
	return  (SUCCESS);
}

t_error	mutexes_init(int philo_nb, t_data *data)
{
	t_index	i;

	if (pthread_mutex_init(&g_at_table_mtx, NULL))
		return (MUTEX_ERROR);
	if (pthread_mutex_init(&g_end_mtx, NULL))
		return (MUTEX_ERROR);
	if (pthread_mutex_init(&data->print_mtx, NULL))
		return (MUTEX_ERROR);
	i = 0;
	while (i < philo_nb)
	{
		if (pthread_mutex_init(&data->forks[i], NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&data->eating_mtx[i], NULL))
			return (MUTEX_ERROR);
		if (pthread_mutex_init(&data->started_mtx[i], NULL))
			return (MUTEX_ERROR);
		i++;
	}
	return (SUCCESS);
}

t_error	memory_alloc(int philo_nb, t_data *data)
{
	if ((data->forks = malloc(sizeof(t_mutex) * philo_nb)) == NULL)
		return (MALLOC_ERROR);
	if ((data->eating_mtx = malloc(sizeof(t_mutex) * philo_nb)) == NULL)
		return (MALLOC_ERROR);
	if ((data->started_mtx = malloc(sizeof(t_mutex) * philo_nb)) == NULL)
		return (MALLOC_ERROR);
	if ((data->philo = malloc(sizeof(t_philo) * philo_nb)) == NULL)
		return (MALLOC_ERROR);
	return (mutexes_init(philo_nb, data) ? MUTEX_ERROR : SUCCESS);
}

int		exit_error(t_error error)
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

t_error	read_args(t_params *parameters, char **argv)
{
	if ((parameters->philo_nb = atoi(argv[1])) < 2)
		return (ARGS_ERROR);
	g_at_table = parameters->philo_nb;
	if ((parameters->time_to_die = atoi(argv[2])) < 1)
		return (ARGS_ERROR);
	if ((parameters->time_to_eat = atoi(argv[3])) < 1)
		return (ARGS_ERROR);
	parameters->time_to_eat *= 1000;
	if ((parameters->time_to_sleep = atoi(argv[4])) < 1)
		return (ARGS_ERROR);
	parameters->time_to_sleep *= 1000;
	if (!argv[5])
		parameters->must_eat_nb = 0;
	else if ((parameters->must_eat_nb = atoi(argv[5])) < 1)
		return (ARGS_ERROR);
	return (SUCCESS);
}

void		wait_simulation_end(int philo_nb)
{
	pthread_mutex_lock(&g_end_mtx);
	pthread_mutex_lock(&g_at_table_mtx);
	while (g_at_table > 0)
	{
		pthread_mutex_unlock(&g_at_table_mtx);
		usleep(100);
		pthread_mutex_lock(&g_at_table_mtx);
	}
	usleep(100 * philo_nb);
	pthread_mutex_unlock(&g_end_mtx);
}

/* 
TODO :

 faire test affichage + segfault pour grand nombre de philosophes

 test une autre version avec solution :
 Si le nombre total est pair :
 tout les philosophes impairs prennent la fourchette n+1 (n etant le numero du philosophe)
 et tout les pair prennent la fourchette n.
 Si le nombre total est impair, je fait l'inverse.
 (prendre i + 1 ? philosophes commençants a 1 ?)

 faire le cécoupage + make
*/

int				main(int argc, char **argv)
{
	t_data		data;
	t_params	parameters;
	t_error		error;

	if (argc < 5 || argc > 6)
		return (exit_error(USAGE));
	if ((error = read_args(&parameters, argv)))
		return (exit_error(error));
	if ((error = memory_alloc(parameters.philo_nb, &data)))
		return (exit_error(error));
	if ((error = str_init(data.str, data.str_len)))
		return (exit_error(error));
	parameters.start_time = get_time();
	if ((error = philo_init(parameters.philo_nb, &data, &parameters)))
		return (exit_error(error));
	if ((error = launch_simulation(parameters.philo_nb, data.philo)))
		return (exit_error(error));
	wait_simulation_end(parameters.philo_nb);
	return (EXIT_SUCCESS);
}