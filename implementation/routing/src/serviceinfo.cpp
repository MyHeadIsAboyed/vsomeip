// Copyright (C) 2014-2016 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "../include/serviceinfo.hpp"

namespace vsomeip {

serviceinfo::serviceinfo(major_version_t _major, minor_version_t _minor,
                         ttl_t _ttl, bool _is_local)
    : group_(0),
      major_(_major),
      minor_(_minor),
      ttl_(0),
      reliable_(nullptr),
      unreliable_(nullptr),
      is_local_(_is_local),
      is_in_mainphase_(false) {

    std::chrono::seconds ttl = static_cast<std::chrono::seconds> (_ttl);
    ttl_ = std::chrono::duration_cast<std::chrono::milliseconds>(ttl);
}

serviceinfo::~serviceinfo() {
}

servicegroup * serviceinfo::get_group() const {
  return group_;
}

void serviceinfo::set_group(servicegroup *_group) {
  group_ = _group;
}

major_version_t serviceinfo::get_major() const {
  return major_;
}

minor_version_t serviceinfo::get_minor() const {
  return minor_;
}

ttl_t serviceinfo::get_ttl() const {
  std::lock_guard<std::mutex> its_lock(ttl_mutex_);
  ttl_t ttl = static_cast<ttl_t>(std::chrono::duration_cast<std::chrono::seconds>(ttl_).count());
  return ttl;
}

void serviceinfo::set_ttl(ttl_t _ttl) {
  std::lock_guard<std::mutex> its_lock(ttl_mutex_);
  std::chrono::seconds ttl = static_cast<std::chrono::seconds>(_ttl);
  ttl_ = std::chrono::duration_cast<std::chrono::milliseconds> (ttl);
}

std::chrono::milliseconds serviceinfo::get_precise_ttl() const {
  std::lock_guard<std::mutex> its_lock(ttl_mutex_);
  return ttl_;
}

void serviceinfo::set_precise_ttl(std::chrono::milliseconds _precise_ttl) {
  std::lock_guard<std::mutex> its_lock(ttl_mutex_);
  ttl_ = _precise_ttl;
}

std::shared_ptr<endpoint> serviceinfo::get_endpoint(bool _reliable) const {
  return (_reliable ? reliable_ : unreliable_);
}

void serviceinfo::set_endpoint(std::shared_ptr<endpoint> _endpoint,
                               bool _reliable) {
  if (_reliable) {
    reliable_ = _endpoint;
  } else {
    unreliable_ = _endpoint;
  }
}

void serviceinfo::add_client(client_t _client) {
  std::lock_guard<std::mutex> its_lock(requesters_mutex_);
  requesters_.insert(_client);
}

void serviceinfo::remove_client(client_t _client) {
  std::lock_guard<std::mutex> its_lock(requesters_mutex_);
  requesters_.erase(_client);
}

uint32_t serviceinfo::get_requesters_size() {
    std::lock_guard<std::mutex> its_lock(requesters_mutex_);
    return static_cast<std::uint32_t>(requesters_.size());
}

bool serviceinfo::is_local() const {
    return is_local_;
}

bool serviceinfo::is_in_mainphase() const {
    return is_in_mainphase_;
}

void serviceinfo::set_is_in_mainphase(bool _in_mainphase) {
    is_in_mainphase_ = _in_mainphase;
}

}  // namespace vsomeip

