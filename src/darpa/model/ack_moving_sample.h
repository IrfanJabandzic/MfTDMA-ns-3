#ifndef ACK_MOVING_SAMPLE_H
#define ACK_MOVING_SAMPLE_H

/*
 * ack_moving_sample.h
 *
 *  Created on: August 3, 2017
 *      Author: Pedro Heleno Isolani
 */

#include <array>
#include <cmath>

#define _stdev(cnt, sum, ssq) sqrt((((double)(cnt))*ssq-pow((double)(sum),2)) / ((double)(cnt)*((double)(cnt)-1)))

template<size_t n>
class ACKMovingSample {
private:
    std::array<unsigned long, n> number_of_sent_packets;
    std::array<unsigned long, n> number_of_received_acks;
    std::array<unsigned long, n> ack_timeout_counters;
    std::array<unsigned long, n> timestamps;
    std::array<double, n>  percentages;

    unsigned int aggregated_sent_packets;
    unsigned int aggregated_received_acks;
    unsigned int aggregated_ack_timeout;

    double percentage_sum;
    double percentage_ssq;
    unsigned int i;
    unsigned int m_size;

public:
    ACKMovingSample() : aggregated_sent_packets(0),
                        aggregated_received_acks(0),
                        aggregated_ack_timeout(0),
                        percentage_sum(0),
                        percentage_ssq(0),
                        i(0),
                        m_size(0) {
        for(unsigned int i = 0; i < n; ++i){
            number_of_sent_packets[i] = 0;
            number_of_received_acks[i] = 0;
            ack_timeout_counters[i] = 0;
            timestamps[i] = 0;
            percentages[i] = 0;
        }
    }

    ~ACKMovingSample() {

    }

    void push(unsigned long current_number_of_sent_packets,
              unsigned long current_number_of_received_acks,
              unsigned long current_ack_timeout_counter,
              unsigned long current_timestamp) {
        //Ex 1, 1, 0. time

        //Aggregated counters remove old value
        aggregated_sent_packets -= number_of_sent_packets[i];
        aggregated_received_acks -= number_of_received_acks[i];
        aggregated_ack_timeout -= ack_timeout_counters[i];

        //Percentage remove old percentage
        percentage_sum -= percentages[i];
        percentage_ssq -= percentages[i] * percentages[i];

        //Aggregate counters re-calc add new sample
        aggregated_sent_packets += current_number_of_sent_packets;
        aggregated_received_acks += current_number_of_received_acks;
        aggregated_ack_timeout += current_ack_timeout_counter;

        //Percentage re-calc
        double current_percentage = 0;
        //If there were packets transmitted during this period
        if(aggregated_sent_packets > 0 && aggregated_received_acks > 0) {
            current_percentage  = ((aggregated_received_acks*100)/aggregated_sent_packets);
        }

        //Assigning new values
        number_of_sent_packets[i] = current_number_of_sent_packets;
        number_of_received_acks[i] = current_number_of_received_acks;
        ack_timeout_counters[i] = current_ack_timeout_counter;
        timestamps[i] = current_timestamp;
        percentages[i] = current_percentage;

        percentage_sum += current_percentage;
        percentage_ssq += current_percentage*current_percentage;
        i = (i+1)%n;
        m_size = m_size < percentages.size() ? m_size + 1 : m_size;
    }

    void removeByIndex(unsigned int i){
        //Aggregate counters re-calc

        aggregated_sent_packets -= number_of_sent_packets[i];
        aggregated_received_acks -= number_of_received_acks[i];
        aggregated_ack_timeout -= ack_timeout_counters[i];

        //Percentage re-calc
        percentage_sum -= percentages[i];
        percentage_ssq -= percentages[i] * percentages[i];
        m_size = m_size <= percentages.size() ? m_size - 1 : m_size;

        //Re-assigning values
        number_of_sent_packets[i] = 0;
        number_of_received_acks[i] = 0;
        ack_timeout_counters[i] = 0;
        timestamps[i] = 0;
        percentages[i] = 0;
    }

    void checkTimeouts(unsigned long threshold, unsigned long current_timestamp){
        for(unsigned int i = 0; i < n; ++i){
            if(timestamps[i] > 0 && (timestamps[i] + threshold) < current_timestamp){
                removeByIndex(i);
            }
        }
    }

    /*void printLists(){
        for(unsigned int i = 0; i < n; ++i){
            std::cout << "i: " << i
                      << " number_of_sent_packets: " << number_of_sent_packets[i]
                      << " number_of_received_acks: " << number_of_received_acks[i]
                      << " ack_timeout_counters: " << ack_timeout_counters[i]
                      << " timestamps[i]: " << timestamps[i]
                      << " percentages[i]: " << percentages[i] << std::endl;
        }
    }*/

    unsigned int size();

    unsigned int get_aggregated_sent_packets();
    unsigned int get_aggregated_received_acks();
    unsigned int get_aggregated_ack_timeout();

    double get_percentage_mean();
    double get_percentage_stdev();
};

template <size_t n>
unsigned int ACKMovingSample<n>::size(){
    return m_size;
}

template <size_t n>
double ACKMovingSample<n>::get_percentage_mean() {
    return size() == 0 ? 0 : percentage_sum/size();
}

template <size_t n>
double ACKMovingSample<n>::get_percentage_stdev() {
    return size() == 0 ? 0 : _stdev(size(), percentage_sum, percentage_ssq);
}

//Aggregated counters
template <size_t n>
unsigned int ACKMovingSample<n>::get_aggregated_sent_packets() {
    return aggregated_sent_packets;
}

template <size_t n>
unsigned int ACKMovingSample<n>::get_aggregated_received_acks() {
    return aggregated_received_acks;
}

template <size_t n>
unsigned int ACKMovingSample<n>::get_aggregated_ack_timeout() {
    return aggregated_ack_timeout;
}

#endif /* ACK_MOVING_SAMPLE_H */
