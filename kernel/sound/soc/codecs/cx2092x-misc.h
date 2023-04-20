
#ifndef _CX2092X_MISC_H_
#define _CX2092X_MISC_H_

struct cx2092_mic_rms {
    unsigned int mic1;
   	unsigned int mic2;
};

#define CX2092X_IOCTL_MAGIC_NUMBER              'c'
#define CX2092X_IOCTL_GET_RMS_BE                _IOR(CX2092X_IOCTL_MAGIC_NUMBER, 1, struct cx2092_mic_rms)
#define CX2092X_IOCTL_GET_RMS_AF                _IOR(CX2092X_IOCTL_MAGIC_NUMBER, 2, struct cx2092_mic_rms)
#define CX2092X_IOCTL_GET_ANGLE_OF_ARRIVAL      _IOR(CX2092X_IOCTL_MAGIC_NUMBER, 3, unsigned int)

#define CX2092X_IOCTL_MAXNR						3

#endif /* _CX2092X_MISC_H_ */
