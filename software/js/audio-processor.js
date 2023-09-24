// audio-processor.js
class PCMProcessor extends AudioWorkletProcessor {
  static get parameterDescriptors() {
    return [];
  }

  constructor() {
    super();
  }

  process(inputs, outputs, parameters) {
    const input = inputs[0];
    const channelData = input[0];
    this.port.postMessage(channelData);
    return true;
  }
}

registerProcessor('pcm-processor', PCMProcessor);
