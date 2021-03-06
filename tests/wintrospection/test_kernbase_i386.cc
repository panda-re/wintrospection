#include <unistd.h>
#include <set>
#include "gtest/gtest.h"
#include "offset/offset.h"
#include "iohal/memory/virtual_memory.h"
#include "wintrospection/wintrospection.h"

// Include an internal header
#include "wintrospection/utils.h"


char* testfile = nullptr;

TEST(WintroKernelbaseTest, Win7SP1i386)
{
    ASSERT_TRUE(testfile) << "Couldn't load input test file!";
    ASSERT_TRUE(access(testfile, R_OK) == 0) << "Could not read input file";

    struct WindowsKernelDetails kdetails = {0};
    struct WindowsKernelOSI kosi = {0};
    kdetails.pointer_width = 4;
    kdetails.kpcr = 0x82933c00;
    pm_addr_t asid = 0x185000;
    bool pae = false;

    kosi.pmem = load_physical_memory_snapshot(testfile);
    kosi.kernel_tlib = load_type_library("windows-32-7sp1");
    ASSERT_TRUE(kosi.pmem != nullptr) << "failed to load physical memory snapshot";
    ASSERT_TRUE(kosi.kernel_tlib!= nullptr) << "failed to load type library";


    ASSERT_TRUE(initialize_windows_kernel_osi(&kosi, &kdetails, asid, pae)) << "Failed to initialize kernel osi";

    ASSERT_EQ(kdetails.kernelbase, 0x82811000) << "Found the wrong kernel base";

    ASSERT_EQ(kdetails.kdbg, 0x82932c28);
    
    ASSERT_EQ(kdetails.version64, 0x82932c00);

    //ASSERT_EQ(kdetails.PsActiveProcessHead, 0x8294a6d8);

    kosi.system_vmem.reset();
    kosi.pmem->free(kosi.pmem);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if (argc != 2) {
        fprintf(stderr, "usage: %s i386.raw\n", argv[0]);
        return 3;
    }

    testfile = argv[1];

    return RUN_ALL_TESTS();
}
