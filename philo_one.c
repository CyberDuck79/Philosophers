/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_one.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/16 20:22:50 by fhenrion          #+#    #+#             */
/*   Updated: 2020/10/16 22:29:45 by fhenrion         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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

int		    exit_error(t_error error)
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

t_error    get_parameters(t_params *parameters, char **argv)
{
    if ((parameters->philo_nb = atoi(argv[1])) < 2)
		return (ARGS_ERROR);
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

t_error     init_simulation(t_data *simulation)
{
    t_index i;

    if (!(simulation->philo = 
        (t_philo*)malloc(sizeof(t_philo) * simulation->parameters.philo_nb)))
        return (MALLOC_ERROR);
    i = 0;
    while (i < simulation->parameters.philo_nb)
    {
        if ((simulation->philo[i].nb = nb_to_str(i + 1)) == NULL)
			return (MALLOC_ERROR);
        //simulation->philo[i].eat_mtx
        //simulation->philo[i].philo_mtx
    }
}

int         main(int argc, char **argv)
{
    t_data      simulation;
    t_error     error;

    if (argc < 5 || argc > 6)
        return (exit_error(USAGE));
    if (get_parameters((t_params*)&simulation.parameters, argv))
        return (exit_error(ARGS_ERROR));
    if ((error = init_simulation(&simulation)))
        return (exit_error(error));
    if ((error = start_simulation(&simulation)))
        return (exit_error(error));
    pthread_mutex_lock(&simulation.end_sim_mtx);
    pthread_mutex_unlock(&simulation.end_sim_mtx);
    return (EXIT_SUCCESS);
}