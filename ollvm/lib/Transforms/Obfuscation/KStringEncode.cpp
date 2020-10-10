//
// Created by king on 2020/10/7.
//


#include <kllvm/Transforms/Obfuscation/Utils.h>
#include "kllvm/Transforms/Obfuscation/KStringEncode.h"

#include <string>
using namespace llvm;

namespace {
    const int defaultKStringSize = 0x10;
    static cl::opt<int>
            KStringSize("kstr_size", cl::desc("Choose the probability [%] each basic blocks will be obfuscated by the -kstr pass"), cl::value_desc("king string encode Encryption length"), cl::init(defaultKStringSize), cl::Optional);

    struct KStringEncode: public FunctionPass{
        static char ID; // Pass identification
        bool flag;
        KStringEncode() : FunctionPass(ID) {}
        KStringEncode(bool flag) : FunctionPass(ID) {this->flag = flag; KStringEncode();}
        virtual bool runOnFunction(Function &F){
            if ( !((KStringSize > 0) && (KStringSize <= 100)) ) {
                errs()<<"KStringEncode application basic blocks percentage -kstr_size=x must be 0 < x <= 100";
                return false;
            }
            if(toObfuscate(flag,&F,"kstr")) {
                kstr(F);
//                printFunction(F);

                return true;
            }
            return false;
        }

        std::string ConvertOpToString(Value* op){
            GlobalVariable* globalVar= dyn_cast<GlobalVariable>(op);
            if(!globalVar){
                errs()<<"dyn cast gloabl err";
                return "";
            }
            ConstantDataSequential* cds=dyn_cast<ConstantDataSequential>(globalVar->getInitializer());
            if(!cds){
                errs()<<"dyn cast constant data err";
                return "";
            }
            return cds->getRawDataValues();;
        }

        void kstr(Function& func){
            //list<std::string>opdata=new list<std::string>();
            Instruction* begin_ins=nullptr;
            vector<string> opdata;
            int idx=0;
            for(BasicBlock& bb:func){
                for(Instruction& ins :bb){
                    if(begin_ins==nullptr){
                        begin_ins=&ins;
                    }
                    for(Value* val:ins.operands()){
                        Value* stripOp=val->stripPointerCasts();
                        if(stripOp->getName().contains(".str")){
                            vector<std::string>::iterator itr= find(opdata.begin(),opdata.end(),stripOp->getName());
                            if(itr!=opdata.end()){
                                continue;
                            }
                            string opname=stripOp->getName();
                            string newname=opname+".arr";
                            opdata.push_back(opname);
                            opdata.push_back(newname);
                            errs()<<*stripOp<<"\n";
                            std::string strdata= ConvertOpToString(stripOp);
                            if(strdata.size()<=0){
                                continue;
                            }
                            errs()<<strdata<<"\n";
                            uint8_t eor_key=llvm::cryptoutils->get_uint8_t();
                            for(int i=0;i<strdata.size();i++){
                                strdata[i]=strdata[i]^eor_key;
                            }
                            //创建一个成员为int的array类型
                            ArrayType* arrType=ArrayType::get(Type::getInt8Ty(func.getContext()),strdata.size());
                            //创建一个申请array空间的指令代码
                            AllocaInst* arrayInst=new AllocaInst(arrType,0,nullptr,1,Twine(stripOp->getName()+".arr"),begin_ins);
//                            errs()<<*arrayInst<<"\n";
                            //创建一个bitcast的指令代码
                            Twine* twine_bitcast = new Twine(stripOp->getName() + ".bitcast");
                            BitCastInst* bitInst=new BitCastInst(arrayInst,Type::getInt8PtrTy(func.getParent()->getContext()),twine_bitcast->str(),begin_ins);
//                            errs()<<*bitInst<<"\n";
                            ConstantInt* eor_data=ConstantInt::get(Type::getInt8Ty(func.getContext()),eor_key);
//                            errs()<<*eor_data<<"\n";
                            AllocaInst* eor_alloc=new AllocaInst(Type::getInt8Ty(func.getContext()),0,nullptr,1,Twine(stripOp->getName()+".xor"),begin_ins);
                            StoreInst* store_eor=new StoreInst(eor_data,eor_alloc);
                            //由于最后的参数是插入在某个指令的前面。我们这里想插入在申请存放eorkey变量的后面，所以这里另外一行插入
                            store_eor->insertAfter(eor_alloc);
                            LoadInst* eor_load=new LoadInst(eor_alloc,"");
                            eor_load->insertAfter(store_eor);

                            for(int i=0;i<strdata.size();i++){
                                ConstantInt* i_const=ConstantInt::get(Type::getInt8Ty(func.getContext()),i);
                                GetElementPtrInst* element=GetElementPtrInst::CreateInBounds(bitInst,i_const);
                                element->insertBefore(begin_ins);
                                ConstantInt* enc_const=ConstantInt::get(Type::getInt8Ty(func.getContext()),strdata[i]);
                                BinaryOperator* binOp=BinaryOperator::CreateXor(enc_const,eor_load);
                                binOp->insertAfter(element);
                                StoreInst* store_data=new StoreInst(binOp,element);
                                store_data->insertAfter(binOp);
                            }
                            val->replaceAllUsesWith(bitInst);
                            GlobalVariable* globalVar= dyn_cast<GlobalVariable>(stripOp);
                            globalVar->eraseFromParent();
                        }
                    }
                }
            }
        }
    };

}
char KStringEncode::ID = 0;
static RegisterPass<KStringEncode> X("kstr", "inserting bogus control flow");

Pass *llvm::createKStringEncode() {
    return new KStringEncode();
}

Pass *llvm::createKStringEncode(bool flag) {
    return new KStringEncode(flag);
}

