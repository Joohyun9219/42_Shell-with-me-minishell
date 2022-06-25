/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_err.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanghyki <kanghyki@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/24 09:35:36 by kanghyki          #+#    #+#             */
/*   Updated: 2022/06/25 11:45:41 by kanghyki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	ft_system_err(enum e_system_err error_code)
{
	ft_putstr_fd("System error: ", 2);
	if (error_code == FAILED_PIPE)
		ft_putendl_fd("pipe() failed, EXIT", 2);
	exit(-1);
}
