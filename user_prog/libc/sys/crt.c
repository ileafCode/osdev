
extern int main(int, char **);

void _start()
{
    unsigned long long code = main(0, 0);
    asm volatile ("movq %0, %%rbx; int $0x85" : : "r"(code)); // Kills this process
    while(1);
}