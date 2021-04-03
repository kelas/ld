#ifndef TIMING_H
#define TIMING_H

#ifdef __linux__
#include<asm/unistd.h>       // for __NR_perf_event_open
#include<linux/perf_event.h> // for perf event constants
#include<sys/ioctl.h>        // for ioctl
#include<unistd.h>           // for syscall

#include<errno.h>   // errno
#include<string.h> // memset

static std::vector<uint64_t> results;
static std::vector<uint64_t> temp_result_vec; // reused rather than allocated in the middle of timing

static int num_phases;
static int num_events;
static int fd;
static int working;

int TimingAccumulator(int num_phases_in, int*config_vec, int config_vec_size) {
  num_phases = num_phases_in;
  working = 1;
  fd = 0;

  perf_event_attr attribs;
  uint64_t ids[config_vec_size];

  memset(&attribs,0,sizeof(attribs));

  attribs.type = PERF_TYPE_HARDWARE; // for now
  attribs.size = sizeof(attribs);
  attribs.disabled = 1;
  attribs.exclude_kernel = 1;
  attribs.exclude_hv = 1;
  attribs.sample_period = 0;
  attribs.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

  const int pid = 0;  // the current process
  const int cpu = -1; // all CPUs
  const unsigned long flags = 0;

  int group = -1; // no group
  num_events = config_vec_size;

  uint32_t i = 0;

  for (auto config : config_vec) {

    attribs.config = config;

    fd = syscall(__NR_perf_event_open, &attribs, pid, cpu, group, flags);

    if (fd == -1) {
      report_error("perf_event_open");
    }

    ioctl(fd, PERF_EVENT_IOC_ID, &ids[i++]);

    if (group == -1) {
      group = fd;
    }

  }

  temp_result_vec.resize(num_events * 2 + 1);
  results.resize(num_phases*num_events, config_vec.size());

}

void start(int phase_number) {
  ioctl(fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
  ioctl(fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
}

void stop(int phase_number) {
  ioctl(fd, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP)
  read(fd, temp_result_vec.data(), temp_result_vec.size() * 8);
  // our actual results are in slots 1,3,5, ... of this structure
  // we really should be checking our ids obtained earlier to be safe
  for (uint32_t i = 1; i < temp_result_vec.size(); i += 2)
    results[phase_number * num_events + i/2] += temp_result_vec[i];
}

void dump() {
  for (int i = 0; i < num_phases; i++) {
    for (int j = 0; j < num_events; j++) {
        std::cout << results[i*num_events + j] << " ";
    }
    std::cout << "\n";
  }
}


#endif // __linux__

#endif
