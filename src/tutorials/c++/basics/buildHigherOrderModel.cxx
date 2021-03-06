/***********************************************************************
 * Tutorial:     Build Higher-OrderModel
 * Author:       Joerg Hendrik Kappes
 * Date:         04.07.2014
 * Dependencies: None
 *
 * Description:
 * ------------
 * This Example construct a model with 3 variables 
 * each take a label from 0 to 9.
 * This defines a 3 digit number.
 *
 * We add a 3rd-order term over variables which is 0 if this number
 * can not be defided by 3 and one otherwise
 *
 *
 ************************************************************************/

#include <iostream>

#include <opengm/opengm.hxx>
#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/operations/multiplier.hxx>

int main(int argc, char** argv) {
   //*******************
   //** Typedefs
   //*******************
   typedef double                                                               ValueType;          // type used for values
   typedef size_t                                                               IndexType;          // type used for indexing nodes and factors (default : size_t)
   typedef size_t                                                               LabelType;          // type used for labels (default : size_t)
   typedef opengm::Multiplier                                                   OpType;             // operation used to combine terms
   typedef opengm::ExplicitFunction<ValueType,IndexType,LabelType>              ExplicitFunction;   // shortcut for explicite function
   typedef opengm::meta::TypeListGenerator<ExplicitFunction>::type              FunctionTypeList;   // list of all function the model cal use (this trick avoids virtual methods) - here only one
   typedef opengm::DiscreteSpace<IndexType, LabelType>                          SpaceType;          // type used to define the feasible statespace
   typedef opengm::GraphicalModel<ValueType,OpType,FunctionTypeList,SpaceType>  Model;              // type of the model
   typedef Model::FunctionIdentifier                                            FunctionIdentifier; // type of the function identifier


   //*******************
   //** Code
   //*******************

   std::cout << "Start building the model ... "<<std::endl;
   // Build empty Model
   LabelType numbersOfLabels[] = {10, 10, 10};
   Model gm(SpaceType(numbersOfLabels, numbersOfLabels + 3));

   // Add 1st order functions and factors to the model
   for(IndexType variable = 0; variable < gm.numberOfVariables(); ++variable) {
      // construct 1st order function
      const LabelType shape[] = {gm.numberOfLabels(variable)};
      ExplicitFunction f(shape, shape + 1);
      for(LabelType state = 0; state < gm.numberOfLabels(variable); ++state) { 
         f(&state) = ValueType(rand()) / RAND_MAX; // general function interface
         //f(state) = ValueType(rand()) / RAND_MAX; // only works for ExpliciteFunction
      }
      // add function
      FunctionIdentifier id = gm.addFunction(f);
      // add factor
      IndexType variableIndex[] = {variable};
      gm.addFactor(id, variableIndex, variableIndex + 1);
   }

   // add 3rd order function and factors to the model
   {
      IndexType vars[]  = {0,1,2}; 
      LabelType shape[] = {numbersOfLabels[0],numbersOfLabels[1],numbersOfLabels[2]};
      LabelType state[] = {0,0,0};
      ExplicitFunction f(shape, shape + 3);
      for(state[0] = 0; state[0] < gm.numberOfLabels(0); ++state[0]){
         for(state[1] = 0; state[1] < gm.numberOfLabels(1); ++state[1]) { 
            for(state[2] = 0; state[2] < gm.numberOfLabels(2); ++state[2]) {
               if((state[0]+state[1]+state[2]) % 3 == 0)
                  f(state)              = 1;
               else 
                  f(state)              = 0;
            }
         }
      }
      // add function
      FunctionIdentifier fid = gm.addFunction(f);
      // add factor
      gm.addFactor(fid, vars, vars + 3);
   }

   // View some model information
   std::cout << "The model has " << gm.numberOfVariables() << " variables."<<std::endl;
   for(size_t i=0; i<gm.numberOfVariables(); ++i){
      std::cout << " * Variable " << i << " has "<< gm.numberOfLabels(i) << " labels."<<std::endl; 
   } 
   std::cout << "The model has " << gm.numberOfFactors() << " factors."<<std::endl;
   for(size_t f=0; f<gm.numberOfFactors(); ++f){
      std::cout << " * Factor " << f << " has order "<< gm[f].numberOfVariables() << "."<<std::endl; 
   }

   LabelType label0[] = {0,3,3};
   LabelType label1[] = {1,2,8};
   std::cout << "The Labeling (" <<label0[0]<<","<<label0[1]<<","<<label0[2]<<")  has the product "<<gm.evaluate(label0)<<"."<<std::endl;
   std::cout << "The Labeling (" <<label1[0]<<","<<label1[1]<<","<<label1[2]<<")  has the product "<<gm.evaluate(label1)<<"."<<std::endl;
   
}
