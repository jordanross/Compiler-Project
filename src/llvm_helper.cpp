#include "llvm_helper.h"

int compile_to_file()
{
    // Applies only to this scope
    using namespace llvm;
    using namespace llvm::sys;

    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    static llvm::LLVMContext TheContext;
    static llvm::IRBuilder<> Builder(TheContext);
    static std::unique_ptr<llvm::Module> TheModule;

    TheModule = make_unique<Module>("my IR", TheContext);

    // Build a simple IR
    // Set up function main (returns i32, no params)
    std::vector<Type *> Parameters;
    FunctionType *FT =
        FunctionType::get(Type::getInt32Ty(TheContext), Parameters, false);
    Function *F =
        Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());

    // Create basic block of main
    BasicBlock *bb = BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(bb);

    // Return a value
    Value *val = ConstantInt::get(TheContext, APInt(32, 2));
    Builder.CreateRet(val);
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto TargetTriple = sys::getDefaultTargetTriple();
    TheModule->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
        errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TheTargetMachine =
        Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    //auto Filename = "output.o";
    auto Filename = "output.s";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }

    legacy::PassManager pass;
    //auto FileType = TargetMachine::CGFT_ObjectFile;
    auto FileType = TargetMachine::CGFT_AssemblyFile;


    if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
        errs() << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*TheModule);

    dest.flush();

    //outs() << "Wrote " << Filename << "\n";
    return 0;
}
