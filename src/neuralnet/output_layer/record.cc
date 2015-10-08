/************************************************************
*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements.  See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership.  The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License.  You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied.  See the License for the
* specific language governing permissions and limitations
* under the License.
*
*************************************************************/
#include "singa/neuralnet/output_layer/record.h"
#include "singa/proto/common.pb.h"
namespace singa {

void RecordOutputLayer::Setup(const LayerProto& conf,
    const vector<Layer*>& srclayers) {
  OutputLayer::Setup(conf, srclayers);
  CHECK_EQ(srclayers.size(), 1);
}

void RecordOutputLayer::ComputeFeature(int flag,
    const vector<Layer*>& srclayers) {
  if (store_ == nullptr)
    store_ = io::OpenStore(layer_conf_.store_conf().backend(),
        layer_conf_.store_conf().path(), io::kCreate);
  const auto& data = srclayers.at(0)->data(this);
  const auto& label = srclayers.at(0)->aux_data();
  int batchsize = data.shape()[0];
  CHECK_GT(batchsize, 0);
  int dim = data.count() / batchsize;
  if (label.size())
    CHECK_EQ(label.size(), batchsize);
  for (int k = 0; k < batchsize; k++){
    SingleLabelImageRecord image;
    if (label.size())
      image.set_label(label[k]);
    auto* dptr = data.cpu_data() + k * dim;
    for (int i = 0; i < dim; i++)
      image.add_data(dptr[i]);
    std::string val;
    image.SerializeToString(&val);
    store_->Write(std::to_string(inst_++), val);
  }
  store_->Flush();
}
} /* singa */
