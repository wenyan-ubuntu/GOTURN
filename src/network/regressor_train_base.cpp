#include "regressor_train_base.h"

MySolver::MySolver(const std::string& param_file)
    : SGDSlover(param_file) {
}

RegressorTrainBase::RegressorTrainBase(const std::string& solver_file)
    : solver_(solver_file)
{
}