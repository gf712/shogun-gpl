/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2012 Sergey Lisitsyn
 */


#include <shogun/transfer/multitask/MultitaskTraceLogisticRegression.h>
#ifdef USE_GPL_SHOGUN
#include <shogun/lib/malsar/malsar_low_rank.h>
#include <shogun/lib/malsar/malsar_options.h>
#include <shogun/lib/IndexBlockGroup.h>
#include <shogun/lib/SGVector.h>
#include <shogun/features/DotFeatures.h>

namespace shogun
{

CMultitaskTraceLogisticRegression::CMultitaskTraceLogisticRegression() :
	CMultitaskLogisticRegression(), m_rho(0.0)
{
	init();
}

CMultitaskTraceLogisticRegression::CMultitaskTraceLogisticRegression(
     float64_t rho, CDotFeatures* train_features,
     CBinaryLabels* train_labels, CTaskGroup* task_group) :
	CMultitaskLogisticRegression(0.0,train_features,train_labels,(CTaskRelation*)task_group)
{
	set_rho(rho);
	init();
}

void CMultitaskTraceLogisticRegression::init()
{
	SG_ADD(&m_rho,"rho","rho", ParameterProperties::HYPER);
}

void CMultitaskTraceLogisticRegression::set_rho(float64_t rho)
{
	m_rho = rho;
}

float64_t CMultitaskTraceLogisticRegression::get_rho() const
{
	return m_rho;
}

CMultitaskTraceLogisticRegression::~CMultitaskTraceLogisticRegression()
{
}

bool CMultitaskTraceLogisticRegression::train_locked_implementation(SGVector<index_t>* tasks)
{
	SGVector<float64_t> y(m_labels->get_num_labels());
	for (int32_t i=0; i<y.vlen; i++)
		y[i] = ((CBinaryLabels*)m_labels)->get_label(i);

	malsar_options options = malsar_options::default_options();
	options.termination = m_termination;
	options.tolerance = m_tolerance;
	options.max_iter = m_max_iter;
	options.n_tasks = ((CTaskGroup*)m_task_relation)->get_num_tasks();
	options.tasks_indices = tasks;

	malsar_result_t model = malsar_low_rank(
		features, y.vector, m_rho, options);

	m_tasks_w = model.w;
	m_tasks_c = model.c;
	return true;
}

bool CMultitaskTraceLogisticRegression::train_machine(CFeatures* data)
{
	if (data && (CDotFeatures*)data)
		set_features((CDotFeatures*)data);

	ASSERT(features)
	ASSERT(m_labels)
	ASSERT(m_task_relation)

	SGVector<float64_t> y(m_labels->get_num_labels());
	for (int32_t i=0; i<y.vlen; i++)
		y[i] = ((CBinaryLabels*)m_labels)->get_label(i);

	malsar_options options = malsar_options::default_options();
	options.termination = m_termination;
	options.tolerance = m_tolerance;
	options.max_iter = m_max_iter;
	options.n_tasks = ((CTaskGroup*)m_task_relation)->get_num_tasks();
	options.tasks_indices = ((CTaskGroup*)m_task_relation)->get_tasks_indices();

	malsar_result_t model = malsar_low_rank(
		features, y.vector, m_rho, options);

	m_tasks_w = model.w;
	m_tasks_c = model.c;

	SG_FREE(options.tasks_indices);

	return true;
}

}

#endif //USE_GPL_SHOGUN
