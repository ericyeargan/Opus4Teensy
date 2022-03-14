extern void runFrameQueueTests();
extern void runRtpFramerTests();

int main(int argc, char **argv) {
    runFrameQueueTests();
    runRtpFramerTests();
    return 0;
}
