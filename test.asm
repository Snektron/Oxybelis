.section .rodata
    .global _resource_test_fs
    .type _resource_test_fs, @object
    .align 4
_resource_test_fs:
    .incbin "test.fs"
_resource_test_fs_end:
    .global _resource_test_fs_size
    .type _resource_test_fs, @object
    .align 4
_resource_test_fs_size:
    .quad _resource_test_fs_end - _resource_test_fs

    .global _resource_a_test_txt
    .type _resource_a_test_txt, @object
    .align 4
_resource_a_test_txt:
    .incbin "a/test.txt"
_resource_a_test_txt_end:
    .global _resource_a_test_txt_size
    .type _resource_a_test_txt, @object
    .align 4
_resource_a_test_txt_size:
    .quad _resource_a_test_txt_end - _resource_a_test_txt
